#include "Animation.h"
#include "assimp/scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include "AnimationDataStructures.h"

void Animation::Initialize(const aiScene * scene, const unsigned int& animationIndex, std::unordered_map<std::string, BoneKeyFrames>* boneKeyMap)
{
	this->boneKeyMap = boneKeyMap;

	m_GlobalInverseTransform = glm::mat4(
		scene->mRootNode->mTransformation.a1, scene->mRootNode->mTransformation.b1, scene->mRootNode->mTransformation.c1, scene->mRootNode->mTransformation.d1,
		scene->mRootNode->mTransformation.a2, scene->mRootNode->mTransformation.b2, scene->mRootNode->mTransformation.c2, scene->mRootNode->mTransformation.d2,
		scene->mRootNode->mTransformation.a3, scene->mRootNode->mTransformation.b3, scene->mRootNode->mTransformation.c3, scene->mRootNode->mTransformation.d3,
		scene->mRootNode->mTransformation.a4, scene->mRootNode->mTransformation.b4, scene->mRootNode->mTransformation.c4, scene->mRootNode->mTransformation.d4);

	aiAnimation* animation = new aiAnimation(*scene->mAnimations[animationIndex]);
	ticksPerSecond = (float)animation->mTicksPerSecond;

	if (boneKeyMap == nullptr || boneKeyMap->empty())
	{
		boneKeyMap->reserve(animation->mNumChannels);
		for (unsigned int i = 0; i < animation->mNumChannels; i++)
		{
			boneKeyMap->insert(std::pair<std::string, BoneKeyFrames> (animation->mChannels[i]->mNodeName.C_Str(), BoneKeyFrames()));
			std::string NodeName = animation->mChannels[i]->mNodeName.C_Str();
			BoneKeyFrames& boneKey = (*boneKeyMap)[NodeName];
			for (unsigned int j = 0; j < animation->mChannels[i]->mNumPositionKeys; j++)
			{
				boneKey.transformKeyFrames.reserve(animation->mChannels[i]->mNumPositionKeys);
				boneKey.transformKeyFrames.push_back(
					glm::vec3(animation->mChannels[i]->mPositionKeys[j].mValue.x, animation->mChannels[i]->mPositionKeys[j].mValue.y, animation->mChannels[i]->mPositionKeys[j].mValue.z)
				);
				boneKey.transformKeyTimes.push_back((float)animation->mChannels[i]->mPositionKeys[j].mTime);
			}
			for (unsigned int j = 0; j < animation->mChannels[i]->mNumRotationKeys; j++)
			{
				boneKey.rotationKeyFrames.reserve(animation->mChannels[i]->mNumRotationKeys);
				boneKey.rotationKeyFrames.push_back(
					glm::quat(animation->mChannels[i]->mRotationKeys[j].mValue.z,
						animation->mChannels[i]->mRotationKeys[j].mValue.w,
						animation->mChannels[i]->mRotationKeys[j].mValue.x,
						animation->mChannels[i]->mRotationKeys[j].mValue.y)
				);
				boneKey.rotationKeyTimes.push_back((float)animation->mChannels[i]->mRotationKeys[j].mTime);
			}
			for (unsigned int j = 0; j < animation->mChannels[i]->mNumScalingKeys; j++)
			{
				boneKey.scaleKeyFrames.reserve(animation->mChannels[i]->mNumScalingKeys);
				boneKey.scaleKeyFrames.push_back(
					glm::vec3(animation->mChannels[i]->mScalingKeys[j].mValue.x, animation->mChannels[i]->mScalingKeys[j].mValue.y, animation->mChannels[i]->mScalingKeys[j].mValue.z)
				);
				boneKey.scaleKeyTimes.push_back((float)animation->mChannels[i]->mScalingKeys[j].mTime);
			}
		}
	}
}

void Animation::ReadNodeHierarchy(Node* node, const glm::mat4 & parentTransform, std::unordered_map<std::string, BoneData> & boneMap, float& animationTime)
{
	if (node->boneKeyFrames == nullptr && boneKeyMap->find(node->name) != boneKeyMap->end())
		node->boneKeyFrames = &(*boneKeyMap)[node->name];
	glm::mat4 NodeTransformation = node->transform;

	//If there are no animations, exit the function
	if (boneKeyMap == nullptr)
		return;

	if(node->boneKeyFrames) {
		// Interpolate scaling and generate scaling transformation matrix
		glm::vec3 Scaling;
		CalculateIntorpolatedScaling(Scaling, node, boneMap, animationTime);
		glm::mat4 ScalingM;
		ScalingM = glm::scale(glm::mat4(1), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		// Interpolate rotation and generate rotation transformation matrix
		glm::quat RotationQ;
		CalculateInterpolatedRotation(RotationQ, node, boneMap, animationTime);
		glm::mat4 RotationM = glm::mat4(glm::toMat4(RotationQ));

		// Interpolate translation and generate translation transformation matrix
		glm::vec3 Translation;
		CalculateInterpolatedPosition(Translation, node, boneMap, animationTime);
		glm::mat4 TranslationM;
		TranslationM = glm::translate(glm::mat4(1), glm::vec3(Translation.x, Translation.y, Translation.z));
		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;		
	}
	glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;

	//TODO: Try to remove the hash map accesses from this line as well
	if (boneMap.find(node->name) != boneMap.end()) {
		boneMap[node->name].finalTransformation = m_GlobalInverseTransform * GlobalTransformation * boneMap[node->name].GetOffsetTransform();
	}

	for (unsigned int i = 0; i < node->children.size(); i++) {
		ReadNodeHierarchy(&node->children[i], GlobalTransformation, boneMap, animationTime);
	}
}

void Animation::CalculateInterpolatedPosition(glm::vec3& out, Node* node, std::unordered_map<std::string, BoneData>& boneMap, float& animationTime)
{
	BoneKeyFrames* currentBoneKeyFrames = node->boneKeyFrames;

	if (currentBoneKeyFrames->transformKeyFrames.size() == 1) {
		out = glm::vec3(currentBoneKeyFrames->transformKeyFrames[0].x, currentBoneKeyFrames->transformKeyFrames[0].y, currentBoneKeyFrames->transformKeyFrames[0].z);
		return;
	}

	float globalAnimationTime = animationTime;

	//If animationTime is outside the bounds of the animation timeline
	if (currentBoneKeyFrames->transformKeyTimes[currentBoneKeyFrames->transformKeyTimes.size() - 1] < animationTime)
		animationTime = currentBoneKeyFrames->transformKeyTimes[currentBoneKeyFrames->transformKeyTimes.size() - 1] - 0.0001f;

	unsigned int PositionIndex = FindPosition(currentBoneKeyFrames, node, animationTime);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < currentBoneKeyFrames->transformKeyTimes.size());
	float DeltaTime = (float)(currentBoneKeyFrames->transformKeyTimes[NextPositionIndex] - currentBoneKeyFrames->transformKeyTimes[PositionIndex]);
	
	float Factor = (animationTime - currentBoneKeyFrames->transformKeyTimes[PositionIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3& Start = glm::vec3(currentBoneKeyFrames->transformKeyFrames[PositionIndex].x, currentBoneKeyFrames->transformKeyFrames[PositionIndex].y, currentBoneKeyFrames->transformKeyFrames[PositionIndex].z);
	const glm::vec3& End = glm::vec3(currentBoneKeyFrames->transformKeyFrames[NextPositionIndex].x, currentBoneKeyFrames->transformKeyFrames[NextPositionIndex].y, currentBoneKeyFrames->transformKeyFrames[NextPositionIndex].z);
	glm::vec3 Delta = End - Start;
	out = Start + Factor * Delta;

	std::swap(globalAnimationTime, animationTime);
}

void Animation::CalculateInterpolatedRotation(glm::quat& out, Node* node, std::unordered_map<std::string, BoneData>& boneMap, float& animationTime)
{
	BoneKeyFrames* currentBoneKeyFrames = node->boneKeyFrames;
	// we need at least two values to interpolate...

	if (currentBoneKeyFrames->rotationKeyFrames.size() == 1) {
		out = glm::quat(currentBoneKeyFrames->rotationKeyFrames[0].x, currentBoneKeyFrames->rotationKeyFrames[0].y, currentBoneKeyFrames->rotationKeyFrames[0].z, currentBoneKeyFrames->rotationKeyFrames[0].w);
		return;
	}

	float globalAnimationTime = animationTime;

	//If animationTime is after the last keyframe, take the last keyframe
	if (currentBoneKeyFrames->rotationKeyTimes[currentBoneKeyFrames->rotationKeyTimes.size() - 1] < animationTime)
		animationTime = currentBoneKeyFrames->rotationKeyTimes[currentBoneKeyFrames->rotationKeyTimes.size() - 1] - 0.0001f;

	unsigned int RotationIndex = FindRotation(currentBoneKeyFrames, node, animationTime);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < currentBoneKeyFrames->rotationKeyFrames.size());
	float DeltaTime = (float)(currentBoneKeyFrames->rotationKeyTimes[NextRotationIndex] - currentBoneKeyFrames->rotationKeyTimes[RotationIndex]);
	float Factor = (animationTime - currentBoneKeyFrames->rotationKeyTimes[RotationIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::quat& StartRotationQ = glm::quat(currentBoneKeyFrames->rotationKeyFrames[RotationIndex].x, currentBoneKeyFrames->rotationKeyFrames[RotationIndex].y, currentBoneKeyFrames->rotationKeyFrames[RotationIndex].z, currentBoneKeyFrames->rotationKeyFrames[RotationIndex].w);
	const glm::quat& EndRotationQ = glm::quat(currentBoneKeyFrames->rotationKeyFrames[NextRotationIndex].x, currentBoneKeyFrames->rotationKeyFrames[NextRotationIndex].y, currentBoneKeyFrames->rotationKeyFrames[NextRotationIndex].z, currentBoneKeyFrames->rotationKeyFrames[NextRotationIndex].w);
	out = glm::slerp(StartRotationQ, EndRotationQ, Factor);
	out = normalize(out);

	std::swap(globalAnimationTime, animationTime);
}

void Animation::CalculateIntorpolatedScaling(glm::vec3& out, Node* node, std::unordered_map<std::string, BoneData>& boneMap, float& animationTime)
{
	BoneKeyFrames* currentBoneKeyFrames = node->boneKeyFrames;

	if (currentBoneKeyFrames->scaleKeyFrames.size() == 1) {
		out = glm::vec3(currentBoneKeyFrames->scaleKeyFrames[0].x, currentBoneKeyFrames->scaleKeyFrames[0].y, currentBoneKeyFrames->scaleKeyFrames[0].z);
		return;
	}

	float globalAnimationTime = animationTime;

	//If animationTime is after the last keyframe, take the last keyframe
	if (currentBoneKeyFrames->scaleKeyTimes[currentBoneKeyFrames->scaleKeyTimes.size() - 1] < animationTime)
		animationTime = currentBoneKeyFrames->scaleKeyTimes[currentBoneKeyFrames->scaleKeyTimes.size() - 1] - 0.0001f;

	unsigned int ScalingIndex = FindScaling(currentBoneKeyFrames, node, animationTime);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < currentBoneKeyFrames->scaleKeyFrames.size());
	float DeltaTime = (float)(currentBoneKeyFrames->scaleKeyTimes[NextScalingIndex] - currentBoneKeyFrames->scaleKeyTimes[ScalingIndex]);
	float Factor = (animationTime - currentBoneKeyFrames->scaleKeyTimes[ScalingIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3& Start = glm::vec3(currentBoneKeyFrames->scaleKeyFrames[ScalingIndex].x, currentBoneKeyFrames->scaleKeyFrames[ScalingIndex].y, currentBoneKeyFrames->scaleKeyFrames[ScalingIndex].z);
	const glm::vec3& End = glm::vec3(currentBoneKeyFrames->scaleKeyFrames[NextScalingIndex].x, currentBoneKeyFrames->scaleKeyFrames[NextScalingIndex].y, currentBoneKeyFrames->scaleKeyFrames[NextScalingIndex].z);
	glm::vec3 Delta = End - Start;
	out = Start + Factor * Delta;

	std::swap(globalAnimationTime, animationTime);
}

unsigned int Animation::FindPosition(BoneKeyFrames* currentBoneKeyFrames, Node* node, float& animationTime)
{
	for (unsigned int i = node->lastPositionKeyFrame; i < currentBoneKeyFrames->transformKeyTimes.size() - 1; i += sgn(animationTime - currentBoneKeyFrames->transformKeyTimes[node->lastPositionKeyFrame])) {
		if (animationTime < currentBoneKeyFrames->transformKeyTimes[i + 1] && animationTime >= currentBoneKeyFrames->transformKeyTimes[i]) {
			node->lastPositionKeyFrame = i;
			return i;
		}
	}
	return currentBoneKeyFrames->transformKeyFrames.size() - 2;

	assert(0);

	return 0;
}

unsigned int Animation::FindRotation(BoneKeyFrames* currentBoneKeyFrames, Node* node, float& animationTime)
{
	assert(currentBoneKeyFrames->rotationKeyFrames.size() > 0);

	for (unsigned int i = node->lastRotationKeyFrame; i < currentBoneKeyFrames->rotationKeyFrames.size() - 1; i+= sgn(animationTime - currentBoneKeyFrames->rotationKeyTimes[node->lastRotationKeyFrame])) {
		if (animationTime < currentBoneKeyFrames->rotationKeyTimes[i + 1] && animationTime >= currentBoneKeyFrames->rotationKeyTimes[i]) {
			node->lastRotationKeyFrame = i;
			return i;
		}
	}
	return currentBoneKeyFrames->rotationKeyFrames.size() - 2;

	assert(0);

	return 0;
}

unsigned int Animation::FindScaling(BoneKeyFrames* currentBoneKeyFrames, Node* node, float& animationTime)
{
	assert(currentBoneKeyFrames->scaleKeyFrames.size() > 0);

	for (unsigned int i = node->lastScaleKeyFrame; i < currentBoneKeyFrames->scaleKeyFrames.size() - 1; i += sgn(animationTime - currentBoneKeyFrames->scaleKeyTimes[node->lastScaleKeyFrame])) {
		if (animationTime < currentBoneKeyFrames->scaleKeyTimes[i + 1] && animationTime >= currentBoneKeyFrames->scaleKeyTimes[i]) {
			node->lastScaleKeyFrame = i;
			return i;
		}
	}
	return currentBoneKeyFrames->scaleKeyFrames.size() - 2;
	assert(0);

	return 0;
}

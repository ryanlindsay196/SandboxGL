#include "Animation.h"
#include "assimp/scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include "gtc/quaternion.hpp"
#include "Renderables/Mesh.h"
#include "Renderables/Model.h"
#include "AnimationDataStructures.h"

void Animation::Initialize(const aiScene * scene, unsigned int animationIndex)
{
	m_GlobalInverseTransform = glm::mat4(
		scene->mRootNode->mTransformation.a1, scene->mRootNode->mTransformation.b1, scene->mRootNode->mTransformation.c1, scene->mRootNode->mTransformation.d1,
		scene->mRootNode->mTransformation.a2, scene->mRootNode->mTransformation.b2, scene->mRootNode->mTransformation.c2, scene->mRootNode->mTransformation.d2,
		scene->mRootNode->mTransformation.a3, scene->mRootNode->mTransformation.b3, scene->mRootNode->mTransformation.c3, scene->mRootNode->mTransformation.d3,
		scene->mRootNode->mTransformation.a4, scene->mRootNode->mTransformation.b4, scene->mRootNode->mTransformation.c4, scene->mRootNode->mTransformation.d4);

	m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
	animation = new aiAnimation(*scene->mAnimations[animationIndex]);
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		std::pair<std::string, BoneKeyFrames> boneKey(animation->mChannels[i]->mNodeName.C_Str(), BoneKeyFrames());
		boneKeyMap.insert(boneKey);
		for (unsigned int j = 0; j < animation->mChannels[i]->mNumPositionKeys; j++)
		{
			boneKeyMap[boneKey.first].transformKeyFrames.reserve(animation->mChannels[i]->mNumPositionKeys);
			boneKeyMap[boneKey.first].transformKeyFrames.push_back(
				glm::vec3(animation->mChannels[i]->mPositionKeys[j].mValue.x, animation->mChannels[i]->mPositionKeys[j].mValue.y, animation->mChannels[i]->mPositionKeys[j].mValue.z)
			);
			boneKeyMap[boneKey.first].transformKeyTimes.push_back(animation->mChannels[i]->mPositionKeys[j].mTime);
		}
		for (unsigned int j = 0; j < animation->mChannels[i]->mNumRotationKeys; j++)
		{
			boneKeyMap[boneKey.first].rotationKeyFrames.reserve(animation->mChannels[i]->mNumRotationKeys);
			boneKeyMap[boneKey.first].rotationKeyFrames.push_back(
				glm::quat(animation->mChannels[i]->mRotationKeys[j].mValue.z, 
					animation->mChannels[i]->mRotationKeys[j].mValue.w, 
					animation->mChannels[i]->mRotationKeys[j].mValue.x, 
					animation->mChannels[i]->mRotationKeys[j].mValue.y)
			);
			boneKeyMap[boneKey.first].rotationKeyTimes.push_back(animation->mChannels[i]->mRotationKeys[j].mTime);
		}
		for (unsigned int j = 0; j < animation->mChannels[i]->mNumScalingKeys; j++)
		{
			boneKeyMap[boneKey.first].scaleKeyFrames.reserve(animation->mChannels[i]->mNumScalingKeys);
			boneKeyMap[boneKey.first].scaleKeyFrames.push_back(
				glm::vec3(animation->mChannels[i]->mScalingKeys[j].mValue.x, animation->mChannels[i]->mScalingKeys[j].mValue.y, animation->mChannels[i]->mScalingKeys[j].mValue.z)
			);
			boneKeyMap[boneKey.first].scaleKeyTimes.push_back(animation->mChannels[i]->mScalingKeys[j].mTime);
		}
	}
}

void Animation::ReadNodeHierarchy(float animationTime, Node* node, const glm::mat4 & parentTransform, std::unordered_map<std::string, BoneData> & boneMap)
{
	std::string NodeName(node->name);
	glm::mat4 NodeTransformation = node->transform;

	auto pBoneKeyFrame = boneKeyMap.find(NodeName);
	if(pBoneKeyFrame != boneKeyMap.end()) {
		// Interpolate scaling and generate scaling transformation matrix
		glm::vec3 Scaling;
		CalculateIntorpolatedScaling(Scaling, animationTime, NodeName);
		glm::mat4 ScalingM;
		ScalingM = glm::scale(glm::mat4(1), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		// Interpolate rotation and generate rotation transformation matrix
		glm::quat RotationQ;
		CalculateInterpolatedRotation(RotationQ, animationTime, NodeName);
		glm::mat4 RotationM = glm::mat4(glm::toMat4(RotationQ));

		// Interpolate translation and generate translation transformation matrix
		glm::vec3 Translation;
		CalculateInterpolatedPosition(Translation, animationTime, NodeName);
		glm::mat4 TranslationM;
		TranslationM = glm::translate(glm::mat4(1), glm::vec3(Translation.x, Translation.y, Translation.z));
		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;		
	}
	glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;

	if (boneMap.find(NodeName) != boneMap.end()) {
		boneMap[NodeName].finalTransformation = m_GlobalInverseTransform * GlobalTransformation * boneMap[NodeName].GetOffsetTransform();
	}

	for (unsigned int i = 0; i < node->children.size(); i++) {
		ReadNodeHierarchy(animationTime, &node->children[i], GlobalTransformation, boneMap);
	}
}

void Animation::CalculateInterpolatedPosition(glm::vec3& out, float animationTime, const std::string nodeName)
{
	if (boneKeyMap[nodeName].transformKeyFrames.size() == 1) {
		out = glm::vec3(boneKeyMap[nodeName].transformKeyFrames[0].x, boneKeyMap[nodeName].transformKeyFrames[0].y, boneKeyMap[nodeName].transformKeyFrames[0].z);
		return;
	}

	//If animationTime is outside the bounds of the animation timeline
	if (boneKeyMap[nodeName].transformKeyTimes[boneKeyMap[nodeName].transformKeyTimes.size() - 1] < animationTime)
		animationTime = boneKeyMap[nodeName].transformKeyTimes[boneKeyMap[nodeName].transformKeyTimes.size() - 1] - 0.0001f;

	unsigned int PositionIndex = FindPosition(animationTime, nodeName);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < boneKeyMap[nodeName].transformKeyTimes.size());
	float DeltaTime = (float)(boneKeyMap[nodeName].transformKeyTimes[NextPositionIndex] - boneKeyMap[nodeName].transformKeyTimes[PositionIndex]);
	
	float Factor = (animationTime - boneKeyMap[nodeName].transformKeyTimes[PositionIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3& Start = glm::vec3(boneKeyMap[nodeName].transformKeyFrames[PositionIndex].x, boneKeyMap[nodeName].transformKeyFrames[PositionIndex].y, boneKeyMap[nodeName].transformKeyFrames[PositionIndex].z);
	const glm::vec3& End = glm::vec3(boneKeyMap[nodeName].transformKeyFrames[NextPositionIndex].x, boneKeyMap[nodeName].transformKeyFrames[NextPositionIndex].y, boneKeyMap[nodeName].transformKeyFrames[NextPositionIndex].z);
	glm::vec3 Delta = End - Start;
	out = Start + Factor * Delta;
}

void Animation::CalculateInterpolatedRotation(glm::quat& out, float animationTime, const std::string nodeName)
{
	// we need at least two values to interpolate...
	if (boneKeyMap[nodeName].rotationKeyFrames.size() == 1) {
		out = glm::quat(boneKeyMap[nodeName].rotationKeyFrames[0].x, boneKeyMap[nodeName].rotationKeyFrames[0].y, boneKeyMap[nodeName].rotationKeyFrames[0].z, boneKeyMap[nodeName].rotationKeyFrames[0].w);
		return;
	}

	//If animationTime is after the last keyframe, take the last keyframe
	if (boneKeyMap[nodeName].rotationKeyTimes[boneKeyMap[nodeName].rotationKeyTimes.size() - 1] < animationTime)
		animationTime = boneKeyMap[nodeName].rotationKeyTimes[boneKeyMap[nodeName].rotationKeyTimes.size() - 1] - 0.0001f;

	unsigned int RotationIndex = FindRotation(animationTime, nodeName);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < boneKeyMap[nodeName].rotationKeyFrames.size());
	float DeltaTime = (float)(boneKeyMap[nodeName].rotationKeyTimes[NextRotationIndex] - boneKeyMap[nodeName].rotationKeyTimes[RotationIndex]);
	float Factor = (animationTime - boneKeyMap[nodeName].rotationKeyTimes[RotationIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::quat& StartRotationQ = glm::quat(boneKeyMap[nodeName].rotationKeyFrames[RotationIndex].x, boneKeyMap[nodeName].rotationKeyFrames[RotationIndex].y, boneKeyMap[nodeName].rotationKeyFrames[RotationIndex].z, boneKeyMap[nodeName].rotationKeyFrames[RotationIndex].w);
	const glm::quat& EndRotationQ = glm::quat(boneKeyMap[nodeName].rotationKeyFrames[NextRotationIndex].x, boneKeyMap[nodeName].rotationKeyFrames[NextRotationIndex].y, boneKeyMap[nodeName].rotationKeyFrames[NextRotationIndex].z, boneKeyMap[nodeName].rotationKeyFrames[NextRotationIndex].w);
	out = glm::slerp(StartRotationQ, EndRotationQ, Factor);
	out = normalize(out);
}

void Animation::CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const std::string nodeName)
{
	if (boneKeyMap[nodeName].scaleKeyFrames.size() == 1) {
		out = glm::vec3(boneKeyMap[nodeName].scaleKeyFrames[0].x, boneKeyMap[nodeName].scaleKeyFrames[0].y, boneKeyMap[nodeName].scaleKeyFrames[0].z);
		return;
	}

	//If animationTime is after the last keyframe, take the last keyframe
	if (boneKeyMap[nodeName].scaleKeyTimes[boneKeyMap[nodeName].scaleKeyTimes.size() - 1] < animationTime)
		animationTime = boneKeyMap[nodeName].scaleKeyTimes[boneKeyMap[nodeName].scaleKeyTimes.size() - 1] - 0.0001f;

	unsigned int ScalingIndex = FindScaling(animationTime, nodeName);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < boneKeyMap[nodeName].scaleKeyFrames.size());
	float DeltaTime = (float)(boneKeyMap[nodeName].scaleKeyTimes[NextScalingIndex] - boneKeyMap[nodeName].scaleKeyTimes[ScalingIndex]);
	float Factor = (animationTime - boneKeyMap[nodeName].scaleKeyTimes[ScalingIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3& Start = glm::vec3(boneKeyMap[nodeName].scaleKeyFrames[ScalingIndex].x, boneKeyMap[nodeName].scaleKeyFrames[ScalingIndex].y, boneKeyMap[nodeName].scaleKeyFrames[ScalingIndex].z);
	const glm::vec3& End = glm::vec3(boneKeyMap[nodeName].scaleKeyFrames[NextScalingIndex].x, boneKeyMap[nodeName].scaleKeyFrames[NextScalingIndex].y, boneKeyMap[nodeName].scaleKeyFrames[NextScalingIndex].z);
	glm::vec3 Delta = End - Start;
	out = Start + Factor * Delta;
}

unsigned int Animation::FindPosition(float animationTime, std::string nodeName)
{
	for (unsigned int i = boneKeyMap[nodeName].lastPositionKeyFrame; i < boneKeyMap[nodeName].transformKeyTimes.size() - 1; i += sgn(animationTime - boneKeyMap[nodeName].transformKeyTimes[boneKeyMap[nodeName].lastPositionKeyFrame])) {
		if (animationTime < boneKeyMap[nodeName].transformKeyTimes[i + 1] && animationTime >= boneKeyMap[nodeName].transformKeyTimes[i]) {
			boneKeyMap[nodeName].lastPositionKeyFrame = i;
			return i;
		}
	}
	return boneKeyMap[nodeName].transformKeyFrames.size() - 2;

	assert(0);

	return 0;
}

unsigned int Animation::FindRotation(float animationTime, const std::string nodeName)
{
	assert(boneKeyMap[nodeName].rotationKeyFrames.size() > 0);

	for (unsigned int i = boneKeyMap[nodeName].lastRotationKeyFrame; i < boneKeyMap[nodeName].rotationKeyFrames.size() - 1; i+= sgn(animationTime - boneKeyMap[nodeName].rotationKeyTimes[boneKeyMap[nodeName].lastRotationKeyFrame])) {
		if (animationTime < boneKeyMap[nodeName].rotationKeyTimes[i + 1] && animationTime >= boneKeyMap[nodeName].rotationKeyTimes[i]) {
			boneKeyMap[nodeName].lastRotationKeyFrame = i;
			return i;
		}
	}
	return boneKeyMap[nodeName].rotationKeyFrames.size() - 2;

	assert(0);

	return 0;
}

unsigned int Animation::FindScaling(float animationTime, const std::string nodeName)
{
	assert(boneKeyMap[nodeName].scaleKeyFrames.size() > 0);

	for (unsigned int i = boneKeyMap[nodeName].lastScaleKeyFrame; i < boneKeyMap[nodeName].scaleKeyFrames.size() - 1; i += sgn(animationTime - boneKeyMap[nodeName].scaleKeyTimes[boneKeyMap[nodeName].lastScaleKeyFrame])) {
		if (animationTime < boneKeyMap[nodeName].scaleKeyTimes[i + 1] && animationTime >= boneKeyMap[nodeName].scaleKeyTimes[i]) {
			boneKeyMap[nodeName].lastScaleKeyFrame = i;
			return i;
		}
	}
	return boneKeyMap[nodeName].scaleKeyFrames.size() - 2;
	assert(0);

	return 0;
}

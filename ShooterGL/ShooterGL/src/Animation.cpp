#include "Animation.h"
#include "assimp/scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include "gtc/quaternion.hpp"
#include "Renderables/Mesh.h"
//#include "assimp/vector3.h"
//#include "assimp/anim.h"

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
			boneKeyMap[boneKey.first].transformKeyFrames.push_back(
				glm::vec3(animation->mChannels[i]->mPositionKeys[j].mValue.x, animation->mChannels[i]->mPositionKeys[j].mValue.y, animation->mChannels[i]->mPositionKeys[j].mValue.z)
			);
			boneKeyMap[boneKey.first].transformKeyTimes.push_back(animation->mChannels[i]->mPositionKeys[j].mTime);
		}
		for (unsigned int j = 0; j < animation->mChannels[i]->mNumRotationKeys; j++)
		{
			boneKeyMap[boneKey.first].rotationKeyFrames.push_back(
				glm::quat(animation->mChannels[i]->mRotationKeys[j].mValue.x, animation->mChannels[i]->mRotationKeys[j].mValue.y, animation->mChannels[i]->mRotationKeys[j].mValue.z, animation->mChannels[i]->mRotationKeys[j].mValue.w)
			);
			boneKeyMap[boneKey.first].rotationKeyTimes.push_back(animation->mChannels[i]->mPositionKeys[j].mTime);
		}
		for (unsigned int j = 0; j < animation->mChannels[i]->mNumScalingKeys; j++)
		{
			boneKeyMap[boneKey.first].scaleKeyFrames.push_back(
				glm::vec3(animation->mChannels[i]->mScalingKeys[j].mValue.x, animation->mChannels[i]->mScalingKeys[j].mValue.y, animation->mChannels[i]->mScalingKeys[j].mValue.z)
			);
			boneKeyMap[boneKey.first].scaleKeyTimes.push_back(animation->mChannels[i]->mPositionKeys[j].mTime);
		}
	}
	//for(unsigned int i = 0; i < scene->mNumAnimations; i++)
	//	animation->mChannels = new aiNodeAnim(**scene->mAnimations[animationIndex]->mChannels);
}

void Animation::ReadNodeHierarchy(float animationTime, const aiNode * node, const glm::mat4 & parentTransform, std::unordered_map<std::string, BoneData> & boneMap)
{
	std::string NodeName(node->mName.data);
	//TODO: Remove aiXXX classes from this function

	glm::mat4 NodeTransformation = glm::mat4(
		node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
		node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
		node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
		node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4
	);

	//const aiNodeAnim* pNodeAnim = FindNodeAnim(animation, NodeName);

	auto pBoneKeyFrame = boneKeyMap.find(NodeName);//FindNodeBone(boneKeyMap, NodeName);

	//if (pNodeAnim) {
	if(pBoneKeyFrame != boneKeyMap.end()) {
		// Interpolate scaling and generate scaling transformation matrix
		glm::vec3 Scaling;
		CalculateIntorpolatedScaling(Scaling, animationTime, NodeName);
		glm::mat4 ScalingM;
		//ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
		ScalingM = glm::scale(glm::mat4(1), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		// Interpolate rotation and generate rotation transformation matrix
		glm::quat RotationQ;
		CalculateInterpolatedRotation(RotationQ, animationTime, NodeName);
		glm::mat4 RotationM = glm::mat4(glm::toMat4(RotationQ));

		// Interpolate translation and generate translation transformation matrix
		glm::vec3 Translation;
		CalculateInterpolatedPosition(Translation, animationTime, NodeName);
		glm::mat4 TranslationM;
		//TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
		TranslationM = glm::translate(glm::mat4(1), glm::vec3(Translation.x, Translation.y, Translation.z));

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;

	if (boneMap.find(NodeName) != boneMap.end()) {
		//unsigned int BoneIndex = boneMap[NodeName];
		//boneMap[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation *
		//	m_BoneInfo[BoneIndex].BoneOffset;
		//unsigned int BoneIndex = boneMap[NodeName];
		boneMap[NodeName].finalTransformation = m_GlobalInverseTransform * GlobalTransformation *
			boneMap[NodeName].GetOffsetTransform();
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ReadNodeHierarchy(animationTime, node->mChildren[i], GlobalTransformation, boneMap);
	}
}

//const aiNodeAnim* Animation::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
//{
//	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
//		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
//
//		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
//			return pNodeAnim;
//		}
//	}
//
//	return NULL;
//}

void Animation::CalculateInterpolatedPosition(glm::vec3& out, float animationTime, const std::string nodeName)
{
	//if (nodeAnim->mNumPositionKeys == 1) {
	if (boneKeyMap[nodeName].transformKeyFrames.size() == 1) {
		//out = glm::vec3(nodeAnim->mPositionKeys[0].mValue.x, nodeAnim->mPositionKeys[0].mValue.y, nodeAnim->mPositionKeys[0].mValue.z);
		out = glm::vec3(boneKeyMap[nodeName].transformKeyFrames[0].x, boneKeyMap[nodeName].transformKeyFrames[0].y, boneKeyMap[nodeName].transformKeyFrames[0].z);
		return;
	}

	//unsigned int PositionIndex = FindPosition(animationTime, nodeAnim);
	unsigned int PositionIndex = FindPosition(animationTime, nodeName);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	//assert(NextPositionIndex < nodeAnim->mNumPositionKeys);
	assert(NextPositionIndex < boneKeyMap[nodeName].transformKeyTimes.size());
	//float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
	float DeltaTime = (float)(boneKeyMap[nodeName].transformKeyTimes[NextPositionIndex] - boneKeyMap[nodeName].transformKeyTimes[PositionIndex]);
	//float Factor = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
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

	unsigned int RotationIndex = FindRotation(animationTime, nodeName);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < boneKeyMap[nodeName].rotationKeyFrames.size());
	float DeltaTime = (float)(boneKeyMap[nodeName].rotationKeyTimes[NextRotationIndex] - boneKeyMap[nodeName].rotationKeyTimes[RotationIndex]);
	float Factor = (animationTime - boneKeyMap[nodeName].rotationKeyTimes[RotationIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::quat& StartRotationQ = glm::quat(boneKeyMap[nodeName].rotationKeyFrames[RotationIndex].x, boneKeyMap[nodeName].rotationKeyFrames[RotationIndex].y, boneKeyMap[nodeName].rotationKeyFrames[RotationIndex].z, boneKeyMap[nodeName].rotationKeyFrames[RotationIndex].w);
	const glm::quat& EndRotationQ = glm::quat(boneKeyMap[nodeName].rotationKeyFrames[NextRotationIndex].x, boneKeyMap[nodeName].rotationKeyFrames[NextRotationIndex].y, boneKeyMap[nodeName].rotationKeyFrames[NextRotationIndex].z, boneKeyMap[nodeName].rotationKeyFrames[NextRotationIndex].w);
	out = mix(StartRotationQ, EndRotationQ, Factor);
	//out = out.Normalize();
	out = normalize(out);
}

void Animation::CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const std::string nodeName)
{
	if (boneKeyMap[nodeName].scaleKeyFrames.size() == 1) {
		out = glm::vec3(boneKeyMap[nodeName].scaleKeyFrames[0].x, boneKeyMap[nodeName].scaleKeyFrames[0].y, boneKeyMap[nodeName].scaleKeyFrames[0].z);
		return;
	}

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
	for (unsigned int i = 0; i < boneKeyMap[nodeName].transformKeyTimes.size() - 1; i++) {
		if (animationTime < boneKeyMap[nodeName].transformKeyTimes[i + 1]) {
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned int Animation::FindRotation(float animationTime, const std::string nodeName)
{
	assert(boneKeyMap[nodeName].rotationKeyFrames.size() > 0);

	for (unsigned int i = 0; i < boneKeyMap[nodeName].rotationKeyFrames.size() - 1; i++) {
		if (animationTime < boneKeyMap[nodeName].rotationKeyTimes[i + 1]) {
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned int Animation::FindScaling(float animationTime, const std::string nodeName)
{
	assert(boneKeyMap[nodeName].scaleKeyFrames.size() > 0);

	for (unsigned int i = 0; i < boneKeyMap[nodeName].scaleKeyFrames.size() - 1; i++) {
		if (animationTime < boneKeyMap[nodeName].scaleKeyTimes[i + 1]) {
			return i;
		}
	}

	assert(0);

	return 0;
}
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

	const BoneKeyFrames* pBoneKeyFrame = FindNodeBone(boneKeyMap, NodeName);

	//if (pNodeAnim) {
	if(pBoneKeyFrame) {
		// Interpolate scaling and generate scaling transformation matrix
		glm::vec3 Scaling;
		CalculateIntorpolatedScaling(Scaling, animationTime, pNodeAnim);
		glm::mat4 ScalingM;
		//ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
		ScalingM = glm::scale(glm::mat4(1), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		// Interpolate rotation and generate rotation transformation matrix
		glm::quat RotationQ;
		CalculateInterpolatedRotation(RotationQ, animationTime, pNodeAnim);
		glm::mat4 RotationM = glm::mat4(glm::toMat4(RotationQ));

		// Interpolate translation and generate translation transformation matrix
		glm::vec3 Translation;
		CalculateInterpolatedPosition(Translation, animationTime, pNodeAnim);
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

const aiNodeAnim* Animation::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void Animation::CalculateInterpolatedPosition(glm::vec3& out, float animationTime, const aiNodeAnim * nodeAnim)
{
	if (nodeAnim->mNumPositionKeys == 1) {
		out = glm::vec3(nodeAnim->mPositionKeys[0].mValue.x, nodeAnim->mPositionKeys[0].mValue.y, nodeAnim->mPositionKeys[0].mValue.z);
		return;
	}

	unsigned int PositionIndex = FindPosition(animationTime, nodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < nodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3& Start = glm::vec3(nodeAnim->mPositionKeys[PositionIndex].mValue.x, nodeAnim->mPositionKeys[PositionIndex].mValue.y, nodeAnim->mPositionKeys[PositionIndex].mValue.z);
	const glm::vec3& End = glm::vec3(nodeAnim->mPositionKeys[NextPositionIndex].mValue.x, nodeAnim->mPositionKeys[NextPositionIndex].mValue.y, nodeAnim->mPositionKeys[NextPositionIndex].mValue.z);
	glm::vec3 Delta = End - Start;
	out = Start + Factor * Delta;
}

void Animation::CalculateInterpolatedRotation(glm::quat& out, float animationTime, const aiNodeAnim * nodeAnim)
{
	// we need at least two values to interpolate...
	if (nodeAnim->mNumRotationKeys == 1) {
		out = glm::quat(nodeAnim->mRotationKeys[0].mValue.x, nodeAnim->mRotationKeys[0].mValue.y, nodeAnim->mRotationKeys[0].mValue.z, nodeAnim->mRotationKeys[0].mValue.w);
		return;
	}

	unsigned int RotationIndex = FindRotation(animationTime, nodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < nodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::quat& StartRotationQ = glm::quat(nodeAnim->mRotationKeys[RotationIndex].mValue.x, nodeAnim->mRotationKeys[RotationIndex].mValue.y, nodeAnim->mRotationKeys[RotationIndex].mValue.z, nodeAnim->mRotationKeys[RotationIndex].mValue.w);
	const glm::quat& EndRotationQ = glm::quat(nodeAnim->mRotationKeys[NextRotationIndex].mValue.x, nodeAnim->mRotationKeys[NextRotationIndex].mValue.y, nodeAnim->mRotationKeys[NextRotationIndex].mValue.z, nodeAnim->mRotationKeys[NextRotationIndex].mValue.w);
	out = mix(StartRotationQ, EndRotationQ, Factor);
	//out = out.Normalize();
	out = normalize(out);
}

void Animation::CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const aiNodeAnim * nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 1) {
		out = glm::vec3(nodeAnim->mScalingKeys[0].mValue.x, nodeAnim->mScalingKeys[0].mValue.y, nodeAnim->mScalingKeys[0].mValue.z);
		return;
	}

	unsigned int ScalingIndex = FindScaling(animationTime, nodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < nodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(nodeAnim->mScalingKeys[NextScalingIndex].mTime - nodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3& Start = glm::vec3(nodeAnim->mScalingKeys[ScalingIndex].mValue.x, nodeAnim->mScalingKeys[ScalingIndex].mValue.y, nodeAnim->mScalingKeys[ScalingIndex].mValue.z);
	const glm::vec3& End = glm::vec3(nodeAnim->mScalingKeys[NextScalingIndex].mValue.x, nodeAnim->mScalingKeys[NextScalingIndex].mValue.y, nodeAnim->mScalingKeys[NextScalingIndex].mValue.z);
	glm::vec3 Delta = End - Start;
	out = Start + Factor * Delta;
}

unsigned int Animation::FindPosition(float animationTime, const aiNodeAnim * nodeAnim)
{
	for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned int Animation::FindRotation(float animationTime, const aiNodeAnim * nodeAnim)
{
	assert(nodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned int Animation::FindScaling(float animationTime, const aiNodeAnim * nodeAnim)
{
	assert(nodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

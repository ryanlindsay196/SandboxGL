#include "Animation.h"
#include "assimp/scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include "gtc/quaternion.hpp"
#include "Renderables/Mesh.h"
#include "Renderables/Model.h"
#include "AnimationDataStructures.h"


//TODO: Delete
#include <GLFW/glfw3.h>
//TODO: Delete
#include <stdio.h>
//TODO: Delete
#include <iostream>
//TODO: Delete

void Animation::Initialize(const aiScene * scene, unsigned int animationIndex)
{
	m_GlobalInverseTransform = glm::mat4(
		scene->mRootNode->mTransformation.a1, scene->mRootNode->mTransformation.b1, scene->mRootNode->mTransformation.c1, scene->mRootNode->mTransformation.d1,
		scene->mRootNode->mTransformation.a2, scene->mRootNode->mTransformation.b2, scene->mRootNode->mTransformation.c2, scene->mRootNode->mTransformation.d2,
		scene->mRootNode->mTransformation.a3, scene->mRootNode->mTransformation.b3, scene->mRootNode->mTransformation.c3, scene->mRootNode->mTransformation.d3,
		scene->mRootNode->mTransformation.a4, scene->mRootNode->mTransformation.b4, scene->mRootNode->mTransformation.c4, scene->mRootNode->mTransformation.d4);

	m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
	animation = new aiAnimation(*scene->mAnimations[animationIndex]);

	boneKeyMap.reserve(animation->mNumChannels);
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		std::pair<std::string, BoneKeyFrames> boneKey(animation->mChannels[i]->mNodeName.C_Str(), BoneKeyFrames());
		for (unsigned int j = 0; j < animation->mChannels[i]->mNumPositionKeys; j++)
		{
			boneKey.second.transformKeyFrames.reserve(animation->mChannels[i]->mNumPositionKeys);
			boneKey.second.transformKeyFrames.push_back(
				glm::vec3(animation->mChannels[i]->mPositionKeys[j].mValue.x, animation->mChannels[i]->mPositionKeys[j].mValue.y, animation->mChannels[i]->mPositionKeys[j].mValue.z)
			);
			boneKey.second.transformKeyTimes.push_back(animation->mChannels[i]->mPositionKeys[j].mTime);
		}
		for (unsigned int j = 0; j < animation->mChannels[i]->mNumRotationKeys; j++)
		{
			boneKey.second.rotationKeyFrames.reserve(animation->mChannels[i]->mNumRotationKeys);
			boneKey.second.rotationKeyFrames.push_back(
				glm::quat(animation->mChannels[i]->mRotationKeys[j].mValue.z, 
					animation->mChannels[i]->mRotationKeys[j].mValue.w, 
					animation->mChannels[i]->mRotationKeys[j].mValue.x, 
					animation->mChannels[i]->mRotationKeys[j].mValue.y)
			);
			boneKey.second.rotationKeyTimes.push_back(animation->mChannels[i]->mRotationKeys[j].mTime);
		}
		for (unsigned int j = 0; j < animation->mChannels[i]->mNumScalingKeys; j++)
		{
			boneKey.second.scaleKeyFrames.reserve(animation->mChannels[i]->mNumScalingKeys);
			boneKey.second.scaleKeyFrames.push_back(
				glm::vec3(animation->mChannels[i]->mScalingKeys[j].mValue.x, animation->mChannels[i]->mScalingKeys[j].mValue.y, animation->mChannels[i]->mScalingKeys[j].mValue.z)
			);
			boneKey.second.scaleKeyTimes.push_back(animation->mChannels[i]->mScalingKeys[j].mTime);
		}
		boneKeyMap.insert(boneKey);
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
	BoneKeyFrames* currentBoneKeyFrames = &boneKeyMap[nodeName];
	if (currentBoneKeyFrames->transformKeyFrames.size() == 1) {
		out = glm::vec3(currentBoneKeyFrames->transformKeyFrames[0].x, currentBoneKeyFrames->transformKeyFrames[0].y, currentBoneKeyFrames->transformKeyFrames[0].z);
		return;
	}

	//If animationTime is outside the bounds of the animation timeline
	if (currentBoneKeyFrames->transformKeyTimes[currentBoneKeyFrames->transformKeyTimes.size() - 1] < animationTime)
		animationTime = currentBoneKeyFrames->transformKeyTimes[currentBoneKeyFrames->transformKeyTimes.size() - 1] - 0.0001f;

	unsigned int PositionIndex = FindPosition(animationTime, currentBoneKeyFrames);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < currentBoneKeyFrames->transformKeyTimes.size());
	float DeltaTime = (float)(currentBoneKeyFrames->transformKeyTimes[NextPositionIndex] - currentBoneKeyFrames->transformKeyTimes[PositionIndex]);
	
	float Factor = (animationTime - currentBoneKeyFrames->transformKeyTimes[PositionIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3& Start = glm::vec3(currentBoneKeyFrames->transformKeyFrames[PositionIndex].x, currentBoneKeyFrames->transformKeyFrames[PositionIndex].y, currentBoneKeyFrames->transformKeyFrames[PositionIndex].z);
	const glm::vec3& End = glm::vec3(currentBoneKeyFrames->transformKeyFrames[NextPositionIndex].x, currentBoneKeyFrames->transformKeyFrames[NextPositionIndex].y, currentBoneKeyFrames->transformKeyFrames[NextPositionIndex].z);
	glm::vec3 Delta = End - Start;
	out = Start + Factor * Delta;
}

void Animation::CalculateInterpolatedRotation(glm::quat& out, float animationTime, const std::string nodeName)
{
	BoneKeyFrames* currentBoneKeyFrames = &boneKeyMap[nodeName];
	// we need at least two values to interpolate...
	if (currentBoneKeyFrames->rotationKeyFrames.size() == 1) {
		out = glm::quat(currentBoneKeyFrames->rotationKeyFrames[0].x, currentBoneKeyFrames->rotationKeyFrames[0].y, currentBoneKeyFrames->rotationKeyFrames[0].z, currentBoneKeyFrames->rotationKeyFrames[0].w);
		return;
	}

	//If animationTime is after the last keyframe, take the last keyframe
	if (currentBoneKeyFrames->rotationKeyTimes[currentBoneKeyFrames->rotationKeyTimes.size() - 1] < animationTime)
		animationTime = currentBoneKeyFrames->rotationKeyTimes[currentBoneKeyFrames->rotationKeyTimes.size() - 1] - 0.0001f;

	unsigned int RotationIndex = FindRotation(animationTime, currentBoneKeyFrames);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < currentBoneKeyFrames->rotationKeyFrames.size());
	float DeltaTime = (float)(currentBoneKeyFrames->rotationKeyTimes[NextRotationIndex] - currentBoneKeyFrames->rotationKeyTimes[RotationIndex]);
	float Factor = (animationTime - currentBoneKeyFrames->rotationKeyTimes[RotationIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::quat& StartRotationQ = glm::quat(currentBoneKeyFrames->rotationKeyFrames[RotationIndex].x, currentBoneKeyFrames->rotationKeyFrames[RotationIndex].y, currentBoneKeyFrames->rotationKeyFrames[RotationIndex].z, currentBoneKeyFrames->rotationKeyFrames[RotationIndex].w);
	const glm::quat& EndRotationQ = glm::quat(currentBoneKeyFrames->rotationKeyFrames[NextRotationIndex].x, currentBoneKeyFrames->rotationKeyFrames[NextRotationIndex].y, currentBoneKeyFrames->rotationKeyFrames[NextRotationIndex].z, currentBoneKeyFrames->rotationKeyFrames[NextRotationIndex].w);
	out = glm::slerp(StartRotationQ, EndRotationQ, Factor);
	out = normalize(out);
}

void Animation::CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const std::string nodeName)
{
	BoneKeyFrames* currentBoneKeyFrames = &boneKeyMap[nodeName];
	if (currentBoneKeyFrames->scaleKeyFrames.size() == 1) {
		out = glm::vec3(currentBoneKeyFrames->scaleKeyFrames[0].x, currentBoneKeyFrames->scaleKeyFrames[0].y, currentBoneKeyFrames->scaleKeyFrames[0].z);
		return;
	}

	//If animationTime is after the last keyframe, take the last keyframe
	if (currentBoneKeyFrames->scaleKeyTimes[currentBoneKeyFrames->scaleKeyTimes.size() - 1] < animationTime)
		animationTime = currentBoneKeyFrames->scaleKeyTimes[currentBoneKeyFrames->scaleKeyTimes.size() - 1] - 0.0001f;

	unsigned int ScalingIndex = FindScaling(animationTime, currentBoneKeyFrames);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < currentBoneKeyFrames->scaleKeyFrames.size());
	float DeltaTime = (float)(currentBoneKeyFrames->scaleKeyTimes[NextScalingIndex] - currentBoneKeyFrames->scaleKeyTimes[ScalingIndex]);
	float Factor = (animationTime - currentBoneKeyFrames->scaleKeyTimes[ScalingIndex]) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3& Start = glm::vec3(currentBoneKeyFrames->scaleKeyFrames[ScalingIndex].x, currentBoneKeyFrames->scaleKeyFrames[ScalingIndex].y, currentBoneKeyFrames->scaleKeyFrames[ScalingIndex].z);
	const glm::vec3& End = glm::vec3(currentBoneKeyFrames->scaleKeyFrames[NextScalingIndex].x, currentBoneKeyFrames->scaleKeyFrames[NextScalingIndex].y, currentBoneKeyFrames->scaleKeyFrames[NextScalingIndex].z);
	glm::vec3 Delta = End - Start;
	out = Start + Factor * Delta;
}

unsigned int Animation::FindPosition(float animationTime, BoneKeyFrames* currentBoneKeyFrames)
{
	for (unsigned int i = currentBoneKeyFrames->lastPositionKeyFrame; i < currentBoneKeyFrames->transformKeyTimes.size() - 1; i += sgn(animationTime - currentBoneKeyFrames->transformKeyTimes[currentBoneKeyFrames->lastPositionKeyFrame])) {
		if (animationTime < currentBoneKeyFrames->transformKeyTimes[i + 1] && animationTime >= currentBoneKeyFrames->transformKeyTimes[i]) {
			currentBoneKeyFrames->lastPositionKeyFrame = i;
			return i;
		}
	}
	return currentBoneKeyFrames->transformKeyFrames.size() - 2;

	assert(0);

	return 0;
}

unsigned int Animation::FindRotation(float animationTime, BoneKeyFrames* currentBoneKeyFrames)
{
	assert(currentBoneKeyFrames->rotationKeyFrames.size() > 0);

	for (unsigned int i = currentBoneKeyFrames->lastRotationKeyFrame; i < currentBoneKeyFrames->rotationKeyFrames.size() - 1; i+= sgn(animationTime - currentBoneKeyFrames->rotationKeyTimes[currentBoneKeyFrames->lastRotationKeyFrame])) {
		if (animationTime < currentBoneKeyFrames->rotationKeyTimes[i + 1] && animationTime >= currentBoneKeyFrames->rotationKeyTimes[i]) {
			currentBoneKeyFrames->lastRotationKeyFrame = i;
			return i;
		}
	}
	return currentBoneKeyFrames->rotationKeyFrames.size() - 2;

	assert(0);

	return 0;
}

unsigned int Animation::FindScaling(float animationTime, BoneKeyFrames* currentBoneKeyFrames)
{
	assert(currentBoneKeyFrames->scaleKeyFrames.size() > 0);

	for (unsigned int i = currentBoneKeyFrames->lastScaleKeyFrame; i < currentBoneKeyFrames->scaleKeyFrames.size() - 1; i += sgn(animationTime - currentBoneKeyFrames->scaleKeyTimes[currentBoneKeyFrames->lastScaleKeyFrame])) {
		if (animationTime < currentBoneKeyFrames->scaleKeyTimes[i + 1] && animationTime >= currentBoneKeyFrames->scaleKeyTimes[i]) {
			currentBoneKeyFrames->lastScaleKeyFrame = i;
			return i;
		}
	}
	return currentBoneKeyFrames->scaleKeyFrames.size() - 2;
	assert(0);

	return 0;
}

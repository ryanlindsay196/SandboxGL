#include "Animation.h"
#include "assimp/scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include "gtc/quaternion.hpp"
//#include "assimp/vector3.h"
//#include "assimp/anim.h"

void Animation::Initialize()
{
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

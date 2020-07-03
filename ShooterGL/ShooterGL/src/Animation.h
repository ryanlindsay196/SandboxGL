#pragma once

#include <vector>
#include "glm.hpp"

struct aiNode;
struct aiNodeAnim;
struct aiQuaternion;
struct aiVector3D;

class Animation
{
private:
	std::vector<glm::mat4> rotationKeyFrames, transformKeyFrames, scaleKeyFrames;

public:
	void Initialize();
	void CalculateInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);;
	void CalculateInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalculateIntorpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);

	unsigned int FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
	void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& ParentTransform);
};
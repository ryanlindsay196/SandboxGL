#pragma once

#include <vector>
#include "glm.hpp"

struct aiNode;
struct aiNodeAnim;
//class aiQuaternion;
//class aiVector3D;

class Animation
{
private:
	std::vector<glm::mat4> rotationKeyFrames, transformKeyFrames, scaleKeyFrames;

public:
	void Initialize();
	void CalculateInterpolatedPosition(glm::vec3& out, float animationTime, const aiNodeAnim* nodeAnim);;
	void CalculateInterpolatedRotation(glm::quat& out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const aiNodeAnim* nodeAnim);

	unsigned int FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned int FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned int FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
	void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& ParentTransform);
};
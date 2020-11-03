#pragma once

#include <vector>
#include "glm.hpp"
#include <unordered_map>

struct aiNode;
struct aiNodeAnim;
struct aiScene;
struct aiAnimation;
struct Node;
class BoneData;

struct BoneKeyFrames
{
	std::vector<glm::vec3> transformKeyFrames, scaleKeyFrames;
	std::vector<glm::quat> rotationKeyFrames;
	std::vector<float> transformKeyTimes, scaleKeyTimes, rotationKeyTimes;

	unsigned int lastPositionKeyFrame, lastRotationKeyFrame, lastScaleKeyFrame;
};

class Animation
{
private:
	glm::mat4 m_GlobalInverseTransform;
	std::unordered_map<std::string, BoneKeyFrames> boneKeyMap;
public:
	float animationTime;
	int ticksPerSecond;
	template <typename T> int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}

	void Initialize(const aiScene * scene, unsigned int animationIndex);

	void ReadNodeHierarchy(Node* node, const glm::mat4& parentTransform, std::unordered_map<std::string, BoneData> & boneMap);

	void CalculateInterpolatedPosition(glm::vec3& out, const std::string nodeName);
	void CalculateInterpolatedRotation(glm::quat& out, const std::string nodeName);
	void CalculateIntorpolatedScaling(glm::vec3& out, const std::string nodeName);

	unsigned int FindPosition(BoneKeyFrames* currentBoneKeyFrames);
	unsigned int FindRotation(BoneKeyFrames* currentBoneKeyFrames);
	unsigned int FindScaling(BoneKeyFrames* currentBoneKeyFrames);
};
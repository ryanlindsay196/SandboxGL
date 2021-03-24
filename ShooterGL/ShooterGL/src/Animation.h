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
};

class Animation
{
private:
	glm::mat4 m_GlobalInverseTransform;
	std::unordered_map<std::string, BoneKeyFrames>* boneKeyMap;
public:
	int ticksPerSecond;
	template <typename T> int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}

	void Initialize(const aiScene * scene, const unsigned int& animationIndex, std::unordered_map<std::string, BoneKeyFrames>* boneKeyMap);
	//void SetBoneKeyMap(std::unordered_map<std::string, BoneKeyFrames>* newBoneKeyMap);
	void ReadNodeHierarchy(Node* node, const glm::mat4& parentTransform, std::unordered_map<std::string, BoneData> & boneMap, float& animationTime);

	void CalculateInterpolatedPosition(glm::vec3& out, Node* node, std::unordered_map<std::string, BoneData>& boneMap, float& animationTime);
	void CalculateInterpolatedRotation(glm::quat& out, Node* node, std::unordered_map<std::string, BoneData>& boneMap, float& animationTime);
	void CalculateIntorpolatedScaling(glm::vec3& out, Node* node, std::unordered_map<std::string, BoneData>& boneMap, float& animationTime);

	unsigned int FindPosition(BoneKeyFrames* currentBoneKeyFrames, Node* node, float& animationTime);
	unsigned int FindRotation(BoneKeyFrames* currentBoneKeyFrames, Node* node, float& animationTime);
	unsigned int FindScaling(BoneKeyFrames* currentBoneKeyFrames, Node* node, float& animationTime);
};
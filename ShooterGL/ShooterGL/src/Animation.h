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
//class aiQuaternion;
//class aiVector3D;
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
	aiAnimation* animation;
	std::unordered_map<std::string, BoneKeyFrames> boneKeyMap;
public:
	void Initialize(const aiScene * scene, unsigned int animationIndex);

	void ReadNodeHierarchy(float animationTime, Node* node, const glm::mat4& parentTransform, std::unordered_map<std::string, BoneData> & boneMap);
	//const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	//const BoneKeyFrames* FindNodeBone(std::unordered_map<std::string, BoneKeyFrames>, const std::string NodeName);

	void CalculateInterpolatedPosition(glm::vec3& out, float animationTime, const std::string nodeName);
	void CalculateInterpolatedRotation(glm::quat& out, float animationTime, const std::string nodeName);
	void CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const std::string nodeName);

	unsigned int FindPosition(float animationTime, std::string nodeName);
	unsigned int FindRotation(float animationTime, const std::string nodeName);
	unsigned int FindScaling(float animationTime, const std::string nodeName);
	//void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& ParentTransform);
};
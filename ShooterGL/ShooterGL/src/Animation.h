#pragma once

#include <vector>
#include "glm.hpp"
#include <unordered_map>

struct aiNode;
struct aiNodeAnim;
struct aiScene;
struct aiAnimation;
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

	void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform, std::unordered_map<std::string, BoneData> & boneMap);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	const BoneKeyFrames* FindNodeBone(std::unordered_map<std::string, BoneKeyFrames>, const std::string NodeName);

	void CalculateInterpolatedPosition(glm::vec3& out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalculateInterpolatedRotation(glm::quat& out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const aiNodeAnim* nodeAnim);

	unsigned int FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned int FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned int FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
	//void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& ParentTransform);
};
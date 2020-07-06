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

class Animation
{
private:
	std::vector<glm::mat4> rotationKeyFrames, transformKeyFrames, scaleKeyFrames;
	glm::mat4 m_GlobalInverseTransform;
public:
	void Initialize(aiScene* scene);

	void ReadNodeHierarchy(aiScene* scene, float animationTime, const aiNode* node, const glm::mat4& parentTransform, std::unordered_map<std::string, BoneData> & boneMap);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);

	void CalculateInterpolatedPosition(glm::vec3& out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalculateInterpolatedRotation(glm::quat& out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const aiNodeAnim* nodeAnim);

	unsigned int FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned int FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned int FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
	//void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& ParentTransform);
};
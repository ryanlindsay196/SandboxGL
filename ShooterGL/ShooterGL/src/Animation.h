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

	unsigned int lastPositionKeyFrame, lastRotationKeyFrame, lastScaleKeyFrame;
};

class Animation
{
private:
	glm::mat4 m_GlobalInverseTransform;
	aiAnimation* animation;
	std::unordered_map<std::string, BoneKeyFrames> boneKeyMap;
public:
	template <typename T> int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}

	void Initialize(const aiScene * scene, unsigned int animationIndex);

	void ReadNodeHierarchy(float animationTime, Node* node, const glm::mat4& parentTransform, std::unordered_map<std::string, BoneData> & boneMap);
	//const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	//const BoneKeyFrames* FindNodeBone(std::unordered_map<std::string, BoneKeyFrames>, const std::string NodeName);

	void CalculateInterpolatedPosition(glm::vec3& out, float animationTime, const std::string nodeName);
	void CalculateInterpolatedRotation(glm::quat& out, float animationTime, const std::string nodeName);
	void CalculateIntorpolatedScaling(glm::vec3& out, float animationTime, const std::string nodeName);

	glm::mat4 InitTranslationTransform(float x, float y, float z)
	{
		glm::mat4 m;
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = x; m[3][1] = y; m[3][2] = z; m[3][3] = 1.0f;
		return m;
	}

	glm::mat4 InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
	{
		glm::mat4 m;
		m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
		m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
		m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
		m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
		return m;
	}

	unsigned int FindPosition(float animationTime, BoneKeyFrames* currentBoneKeyFrames);
	unsigned int FindRotation(float animationTime, BoneKeyFrames* currentBoneKeyFrames);
	unsigned int FindScaling(float animationTime, BoneKeyFrames* currentBoneKeyFrames);
	//void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& ParentTransform);
};
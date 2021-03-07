#pragma once

#include "../WorldComponent.h"
#include "assimp/matrix4x4.h"

struct BoneKeyFrames;

//Struct of data for each individual bone in a model
class BoneData : public WorldComponent
{
public:
	BoneData() : boneID(0) {}
	BoneData(const unsigned int newID) : boneID(newID) {
	}
	BoneData(const BoneData& other) : boneID(other.boneID) {
		offsetTransform = other.offsetTransform;
		positionOffset = other.positionOffset;
		direction = other.direction;
		finalTransformation = other.finalTransformation;
		componentParent = other.componentParent;
		isActive = other.isActive;
		roll = other.roll;
		pitch = other.pitch;
		yaw = other.yaw;
		rotationQuat = other.rotationQuat;
		scaleOffset = other.scaleOffset;
	}
	const unsigned int boneID;
	glm::mat4 finalTransformation;
};

struct Node
{
	glm::mat4 transform;
	std::vector<Node> children;
	Node* parent;
	std::string name;
	//pointer to animation data(bone keyframes and stuff)
	BoneKeyFrames* boneKeyFrames;
	unsigned int lastPositionKeyFrame, lastRotationKeyFrame, lastScaleKeyFrame;
};

glm::mat4 AiMat4ToGlmMat4(aiMatrix4x4 aiMat4);
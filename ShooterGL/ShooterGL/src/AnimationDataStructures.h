#pragma once

#include "../WorldComponent.h"
#include "assimp/matrix4x4.h"

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
		pitch = other.pitch;
		roll = other.roll;
		rotationQuat = other.rotationQuat;
		scaleOffset = other.scaleOffset;
		yaw = other.yaw;
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
};

glm::mat4 AiMat4ToGlmMat4(aiMatrix4x4 aiMat4);
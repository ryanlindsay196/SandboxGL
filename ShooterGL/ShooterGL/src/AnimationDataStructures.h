#pragma once

#include "../WorldComponent.h"

class BoneData : public WorldComponent
{
public:
	BoneData() : boneID(0) {}
	BoneData(const unsigned int newID) : boneID(newID) {
	}
	BoneData(const BoneData& other) : boneID(other.boneID) {
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
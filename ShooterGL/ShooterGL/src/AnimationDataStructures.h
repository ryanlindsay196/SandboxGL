#pragma once

#include "../WorldComponent.h"

class BoneData : public WorldComponent
{
public:
	unsigned int boneID;
	glm::mat4 finalTransformation;
};

struct Node
{
	glm::mat4 transform;
	std::vector<Node> children;
	Node* parent;
	std::string name;
};
#pragma once
#include <vector>
#include "Renderables/Mesh.h"
#include "AnimationDataStructures.h"

class MeshData
{
public:
	unsigned int VAO, EBO, VBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	glm::mat4 meshTransform;
};

class ModelData
{
public:
	std::vector<MeshData> m_meshData;
	Node* rootNode;
	std::vector<Animation> animations;
	std::unordered_map<std::string, BoneData> boneMap;
	//Contains the position/rotation/scale for each bone, and the frames they occur
	std::unordered_map<std::string, BoneKeyFrames> boneKeyMap;
};
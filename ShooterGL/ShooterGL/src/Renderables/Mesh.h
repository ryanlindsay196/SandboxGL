#pragma once

#include <vector>
#include "glm.hpp"
#include "Shader.h"
#include "Texture.h"
#include <unordered_map>
#include "../WorldComponent.h"
#include "Animation.h"

class ObjectManager;
class ModelData;
class MeshData;
class Animation;
struct aiScene;
struct aiNode;
struct aiMesh;
struct Node;

class BoneData : public WorldComponent
{
public:
	unsigned int boneID;
	glm::mat4 finalTransformation;
};

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int BoneID[4];
	float WeightValue[4];
};

class Mesh : public WorldComponent {
public:
	//mesh data
	//std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;
	Shader* shader;
	//unsigned int VAO;

	//Mesh(ObjectManager* objectManager, std::vector<Vertex> vertices, std::vector<unsigned int> indices, char* materialPath, WorldComponent* newParent);
	Mesh(ObjectManager* objectManager, const aiScene * aiScene, aiMesh* mesh, char* materialPath, WorldComponent* newParent, MeshData* meshData, const aiNode* node);
	Mesh(ObjectManager* objectManager, char* materialPath, WorldComponent* newParent, MeshData* meshData);
	
	void AddNode(aiNode* node, const aiScene* scene, aiNode* oldNode);
	//void ReadMaterial(char* materialPath);
	void LoadShaders();
	void SetDefaultShaders();
	void SetShaders(char* materialPath);
	void AttachMeshData(MeshData* meshData);
	Shader* GetShader();
	void Render(Node* rootNode);
	void SetupMesh();
	virtual void Update(float gameTime) override;

	WorldComponent* parentMesh;

private:
	//unsigned int VBO, EBO;
	TextureManager* textureManager;
	ObjectManager* m_objectManager;
	std::string m_materialPath;
	MeshData* m_meshData;

#include "Animation.h"
	//aiNode* rootNode;

	std::vector<Animation> animations;
	unsigned int animationIndex;

	std::unordered_map<std::string, BoneData> boneMap;
};
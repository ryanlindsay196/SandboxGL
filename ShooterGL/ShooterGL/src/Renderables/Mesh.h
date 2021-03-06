#pragma once

#include <vector>
#include "glm.hpp"
#include "Shader.h"
#include "Texture.h"
#include <unordered_map>
#include "../WorldComponent.h"
#include "Animation.h"
#include <memory>

class ObjectManager;
class ModelData;
class MeshData;
class Animation;
struct aiScene;
struct aiNode;
struct aiMesh;
struct Node;

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
	std::shared_ptr<Shader> shader;

	Mesh(aiMesh* mesh, std::string& materialPath, WorldComponent* newParent, MeshData* meshData, const aiNode* node, std::unordered_map<std::string, BoneData>& boneMap, unsigned int & numBones);
	Mesh(std::string& materialPath, WorldComponent* newParent, MeshData* meshData);

	void LoadShaders();
	void SetDefaultShaders();
	void SetShaders(std::string materialPath);
	void AttachMeshData(MeshData* meshData);
	std::shared_ptr<Shader> GetShader();
	void Render();
	void SetupMesh();
	virtual void Update(float gameTime) override;

	WorldComponent* parentMesh;

private:
	TextureManager* textureManager;
	std::string m_materialPath;
	MeshData* m_meshData;
};
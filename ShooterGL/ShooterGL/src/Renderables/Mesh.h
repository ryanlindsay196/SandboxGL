#pragma once

#include <vector>
#include "glm.hpp"
#include "Shader.h"
#include "Texture.h"
#include "../WorldComponent.h"

class ObjectManager;
class ModelData;
class MeshData;
struct aiScene;
struct aiMesh;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

class Mesh : public WorldComponent {
public:
	//mesh data
	//std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;
	Shader* shader;
	//unsigned int VAO;

	//Mesh(ObjectManager* objectManager, std::vector<Vertex> vertices, std::vector<unsigned int> indices, char* materialPath, WorldComponent* newParent);
	Mesh(ObjectManager* objectManager, const aiScene * aiScene, aiMesh* mesh, char* materialPath, WorldComponent* newParent, ModelData* modelData, MeshData* meshData);

	//void ReadMaterial(char* materialPath);
	void LoadShaders();
	void SetDefaultShaders();
	void SetShaders(char* materialPath);
	Shader* GetShader();
	void Draw();
	void SetupMesh();
	virtual void Update(float gameTime) override;

	WorldComponent* parentMesh;

private:
	//unsigned int VBO, EBO;
	TextureManager* textureManager;
	ObjectManager* m_objectManager;
	std::string m_materialPath;
	ModelData* m_modelData;
	MeshData* m_meshData;
};
#pragma once

#include <vector>
#include "glm.hpp"
#include "Shader.h"
#include "Texture.h"
#include "../WorldComponent.h"

class ObjectManager;

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
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Shader* shader;
	unsigned int VAO;

	Mesh(ObjectManager* objectManager, std::vector<Vertex> vertices, std::vector<unsigned int> indices, char* materialPath, WorldComponent* newParent);

	//void ReadMaterial(char* materialPath);
	void SetDefaultShaders();
	void SetShaders(char * vertexPath, char * fragmentPath, char* materialPath);
	Shader* GetShader();
	void Draw();
	void SetupMesh();

	WorldComponent* parentMesh;

private:
	unsigned int VBO, EBO;
	TextureManager* textureManager;
};
#pragma once

#include <vector>
#include "glm.hpp"
#include "Shader.h"
#include "Texture.h"

class ObjectManager;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

class Mesh {
public:
	//mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Shader* shader;
	unsigned int VAO;

	Mesh(ObjectManager* objectManager, std::vector<Vertex> vertices, std::vector<unsigned int> indices, char* materialPath);

	//void ReadMaterial(char* materialPath);
	void SetDefaultShaders();
	void SetShaders(char * vertexPath, char * fragmentPath, char* materialPath);
	void Draw();
	void SetupMesh();

private:
	unsigned int VBO, EBO;
	TextureManager* textureManager;
};
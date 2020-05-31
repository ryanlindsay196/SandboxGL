#pragma once

#include <vector>
#include "glm.hpp"
#include "Shader.h"
#include "Texture.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	//TODO: add tangents?
	//TODO: add bittangents?
};

class Mesh {
public:
	//mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Shader* shader;
	unsigned int VAO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, char* materialPath);

	void Draw();
	void SetupMesh();

private:
	unsigned int VBO, EBO;
};
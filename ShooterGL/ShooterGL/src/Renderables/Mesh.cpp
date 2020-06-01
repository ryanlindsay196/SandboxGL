#include "Mesh.h"
//#include "GLFW/glfw3.h"
#include "gtc/matrix_transform.hpp"
#include "glad/glad.h"
#include <string>
#include <algorithm>
//#include <fstream>
//#include <iostream>
#include "ManagerClasses/ObjectManager.h"
//TODO: Potentially remove the no warnings thing
//#include "Texture.h"

Mesh::Mesh(ObjectManager* objectManager, std::vector<Vertex> vertices, std::vector<unsigned int> indices, char * materialPath)
{
	textureManager = objectManager->textureManager;;

	this->vertices = vertices;
	this->indices = indices;

	//TODO: When making the shadermanager (like the textureManager), replace this to search for an existing shader
	shader = new Shader();
	if(materialPath)
		shader->Initialize(textureManager, (char*)"", (char*)"", materialPath);
	else
		shader->Initialize(textureManager, (char*)"Shaders/VertexDefault.glsl", (char*)"Shaders/FragmentDefault.glsl", materialPath);
	//this->textures = textures;
	//ReadMaterial(materialPath);
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetupMesh();
}

void Mesh::SetDefaultShaders()
{
	SetShaders((char*)"Shaders/VertexDefault.glsl", (char*)"Shaders/FragmentDefault.glsl", (char*)"");
}

void Mesh::SetShaders(char * vertexPath, char * fragmentPath, char* materialPath)
{
	if (shader != nullptr)
		delete(shader);
	shader = new Shader();
	shader->Initialize(textureManager, vertexPath, fragmentPath, materialPath);

}

Shader * Mesh::GetShader()
{
	return shader;
}

void Mesh::Draw()
{
	shader->UseShader();
	shader->BindTextures();
	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, indices.size());
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

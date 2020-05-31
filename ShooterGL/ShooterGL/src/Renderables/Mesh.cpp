#include "Mesh.h"
//#include "GLFW/glfw3.h"
#include "gtc/matrix_transform.hpp"
#include "glad/glad.h"
#include <string>
//#include "Texture.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, char * materialPath)
{
	this->vertices = vertices;
	this->indices = indices;
	//this->textures = textures;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetupMesh();
}

void Mesh::Draw()
{
	shader->BindTextures();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{

#pragma region ToRemove?

	// bind appropriate textures
	//unsigned int diffuseNr = 1;
	//unsigned int specularNr = 1;
	//unsigned int normalNr = 1;
	//unsigned int heightNr = 1;
	//for (unsigned int i = 0; i < shader->GetTextures().size(); i++)
	//{
		//glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		//std::string number;
		//std::string name = textures[i].type;
		//if (name == "texture_diffuse")
		//	number = std::to_string(diffuseNr++);
		//else if (name == "texture_specular")
		//	number = std::to_string(specularNr++); // transfer unsigned int to stream
		//else if (name == "texture_normal")
		//	number = std::to_string(normalNr++); // transfer unsigned int to stream
		//else if (name == "texture_height")
		//	number = std::to_string(heightNr++); // transfer unsigned int to stream
		
		// now set the sampler to the correct texture unit
		//glUniform1i(glGetUniformLocation(shader->GetShaderProgram(), (name + number).c_str()), i);
		// and finally bind the texture
		//glBindTexture(GL_TEXTURE_2D, textures[i].id);
	//}
#pragma endregion
	shader->BindTextures();

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

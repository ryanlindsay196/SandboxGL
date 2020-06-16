#include "Mesh.h"
//#include "GLFW/glfw3.h"
#include "gtc/matrix_transform.hpp"
#include "glad/glad.h"
#include <string>
#include <algorithm>
#include "ManagerClasses/ObjectManager.h"
#include "ManagerClasses/ShaderManager.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"

Mesh::Mesh(ObjectManager* objectManager, std::vector<Vertex> vertices, std::vector<unsigned int> indices, char * materialPath, WorldComponent* newParent)
{
	parentMesh = newParent;
	yaw = -90;
	textureManager = objectManager->textureManager;
	m_objectManager = objectManager;

	scaleOffset = glm::mat4(1);
	rotationQuat = glm::quat();
	positionOffset = glm::mat4(1);

	offsetTransform = glm::mat4(1);

	this->vertices = vertices;
	this->indices = indices;
	m_materialPath = materialPath;
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetupMesh();
}

void Mesh::LoadShaders()
{
	if (m_materialPath != "")
		SetShaders((char*)m_materialPath.c_str());
	else
		shader->Initialize(m_objectManager, (char*)"Shaders/VertexDefault.glsl", (char*)"Shaders/FragmentDefault.glsl", (char*)m_materialPath.c_str());
}

void Mesh::SetDefaultShaders()
{
	SetShaders((char*)"Resources/Materials/DefaultMaterial.mat");
}

void Mesh::SetShaders(char* materialPath)
{
	//if (shader != nullptr)
	//	delete(shader);
	//shader = new Shader();
	shader = m_objectManager->shaderManager->LoadNewShader(materialPath, m_objectManager);
	//shader->Initialize(textureManager, vertexPath, fragmentPath, materialPath);

}

Shader * Mesh::GetShader()
{
	return shader;
}

//TODO: Change name to Render()
void Mesh::Draw()
{
	//if (parentMesh->componentParent != nullptr)
	//	offsetTransform = parentMesh->componentParent->GetTransform();
	//offsetTransform *= positionOffset * glm::toMat4(rotationQuat) * scaleOffset;
	
	//shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform * parentMesh->GetOffsetTransform());
	//shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform);
	//shader->SetShaderUniform_mat4fv((char*)"model", parentMesh->componentParent->GetTransform() * offsetTransform);
	//shader->SetShaderUniform_mat4fv((char*)"model", parentMesh->GetOffsetTransform() * offsetTransform);
	//RotateQuaternion(glm::vec3(1, 1, 1), 0.2f);
	//shader->SetShaderUniform_mat4fv((char*)"model", parentMesh->componentParent->GetTransform());
	//shader->SetShaderUniform_mat4fv((char*)"model", glm::mat4(1));


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
	//TODO: modify vertex attribute pointers based on material/shader?
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

void Mesh::Update(float gameTime)
{
	//SetTransform(parentMesh->componentParent->GetTransform());
}

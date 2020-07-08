#include "Model.h"
#include "Shader.h"
#include <glad/glad.h>
#include "ManagerClasses/TextureManager.h"
#include "ManagerClasses/ObjectManager.h"
#include "ManagerClasses/CameraManager.h"
#include "ManagerClasses/LightManager.h"
#include "ManagerClasses/ModelManager.h"
#include "Mesh.h"
#include "ModelData.h"
#include "matrix.hpp"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model()
{

}

Model::~Model()
{
	//for (unsigned int i = 0; i < m_meshes.size(); i++)
	//	delete(&m_meshes[i]);
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
}

void Model::Initialize(ObjectManager* objectManager, glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset, char * modelPath, char * materialPath)
{
	//yaw = -90;
	m_textureManager = objectManager->textureManager;
	m_objectManager = objectManager;

	m_modelData = objectManager->modelManager->LoadModelData(modelPath);

	if(m_modelData->m_meshData.size() != 0)
	{
		for (unsigned int i = 0; i < m_modelData->m_meshData.size(); i++)
		{
			//if (m_meshes.size() < m_modelData->m_meshData.size())
			m_meshes.push_back(Mesh(objectManager, materialPath, this, &m_modelData->m_meshData[i]));
			m_meshes[m_meshes.size() - 1].SetTransform(m_modelData->m_meshData[i].meshTransform);
		}
	}
	else
		LoadModel(modelPath, materialPath);
	for(unsigned int i = 0; i < m_meshes.size(); i++)
		m_meshes[i].AttachMeshData(&m_modelData->m_meshData[i]);

	offsetTransform = glm::mat4(1);
	positionOffset = glm::mat4(1);
	scaleOffset = glm::mat4(1);
	scaleOffset = glm::scale(scaleOffset, initialScaleOffset);
	rotationQuat = glm::quat(0, 0, 0, 1);
	rotationQuat *= glm::angleAxis(rotationAngle, rotationAxis);

	//Rotate imported models right-side up
	rotationQuat = glm::rotate(rotationQuat, glm::vec3(M_PI / 2, 0, 0));
	positionOffset = glm::translate(positionOffset, initialPositionOffset);
	offsetTransform = positionOffset * glm::toMat4(rotationQuat) * scaleOffset;

	if (componentParent != nullptr)
		offsetTransform *= componentParent->GetTransform();
}

void Model::LoadModel(std::string modelPath, std::string materialPath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	//check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) //if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	
	//retrieve the directory path of the filepath
	directory = modelPath.substr(0, modelPath.find_last_of('/'));

	//process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, materialPath, &rootNode, nullptr);
}

void Model::ProcessNode(aiNode * node, const aiScene * scene, std::string materialPath, Node* currentNode, Node* parentNode)
{
	currentNode->name = node->mName.C_Str();
	currentNode->transform = glm::mat4(node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
		node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
		node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
		node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4
	);
	currentNode->parent = parentNode;

	//process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//the node object only contains indices to index the actual objects in the scene.
		//the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene, (char*)materialPath.c_str(), node));
		m_meshes[m_meshes.size() - 1].SetTransform(
			glm::mat4(node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
				node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
				node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
				node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4
		));
		m_modelData->m_meshData[m_meshes.size() - 1].meshTransform = m_meshes[m_meshes.size() - 1].GetOffsetTransform();
	}
	//after we've processed all of the meshes (if any) when recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		currentNode->children.push_back(Node());
		ProcessNode(node->mChildren[i], scene, (char*)materialPath.c_str(), &currentNode->children[i], currentNode);
	}
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene, char* materialPath, const aiNode* node)
{
	if (m_meshes.size() == m_modelData->m_meshData.size())
		m_modelData->m_meshData.push_back(MeshData());
	return Mesh(m_objectManager, scene, mesh, materialPath, this, &(m_modelData->m_meshData[m_meshes.size()]), node);
}

void Model::LoadShaders()
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].LoadShaders();
	}
}

unsigned int Model::GetLoadedMeshesCount()
{
	return m_meshes.size();
}

Mesh * Model::GetMesh(unsigned int i)
{
	if (i < m_meshes.size())
		return &m_meshes[i];
	return nullptr;
}

void Model::Update(float gameTime)
{
	//offsetTransform = glm::mat4(1);
	//offsetTransform = glm::scale(offsetTransform, initialScaleOffset);
	//offsetTransform = glm::rotate(offsetTransform, rotationAngle, rotationAxis);
	//offsetTransform = glm::translate(offsetTransform, positionOffset);
	WorldComponent::Update(gameTime);
	for (Mesh mesh : m_meshes)
	{
		mesh.Update(gameTime);
		//mesh.SetTransform(componentParent->GetTransform());
	}
}

void Model::Render()
{
	//glm::vec3 tempVec = glm::vec3(0, 1, 0);
	//shader->SetShaderUniform_vec3((char*)"lightPos", tempVec);
	offsetTransform = positionOffset * glm::toMat4(rotationQuat) * scaleOffset;

	//shader->SetShaderUniform_mat4fv((char*)"view", m_objectManager->cameraManager->GetCamera(0)->viewMatrix);
	//shader->SetShaderUniform_mat4fv((char*)"projection", m_objectManager->cameraManager->GetCamera(0)->projectionMatrix);
	//if (componentParent != nullptr)
	//{
	//	shader->SetShaderUniform_mat4fv((char*)"model", componentParent->GetTransform() * offsetTransform);
	//}
	//else
	//{
	//	shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform);
	//}
	////Drawing code (in render loop)
	//glUseProgram(shader->GetShaderProgram());
	//glm::vec3 pointLightPositions[] = {
	//glm::vec3(-1.7f,  0.2f,  2.0f),
	//glm::vec3(-2.3f, -3.3f, -4.0f),
	//glm::vec3(-4.0f,  2.0f, -12.0f),
	//glm::vec3(0.0f,  0.0f, -3.0f)
	//};
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[0].constant", 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"dirLight.direction", -0.2f, -1.0f, -0.3f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"dirLight.ambient", 0.05f, 0.05f, 0.05f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"dirLight.specular", 0.5f, 0.5f, 0.5f);
	//// point light 1
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[0].position", pointLightPositions[0]);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[0].constant", 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[0].linear", 0.09);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[0].quadratic", 0.032);
	//// point light 2
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[1].position", pointLightPositions[1]);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[1].constant", 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[1].linear", 0.09);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[1].quadratic", 0.032);
	//// point light 3
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[2].position", pointLightPositions[2]);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[2].constant", 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[2].linear", 0.09);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[2].quadratic", 0.032);
	//// point light 4
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[3].position", pointLightPositions[3]);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	//m_meshes[0].shader->SetShaderUniform_vec3((char*)"pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[3].constant", 1.0f);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[3].linear", 0.09);
	//m_meshes[0].shader->SetShaderUniform_vec1((char*)"pointLights[3].quadratic", 0.032);
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"view", m_objectManager->cameraManager->GetCamera(0)->viewMatrix);
		m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"projection", m_objectManager->cameraManager->GetCamera(0)->projectionMatrix);
		if (componentParent != nullptr)
		{
			m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"model", componentParent->GetTransform() * offsetTransform * m_meshes[i].GetOffsetTransform());
		}
		else
		{
			m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform * m_meshes[i].GetOffsetTransform());
			//m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"model", glm::translate(glm::scale(m_meshes[i].GetOffsetTransform(), glm::vec3(0.02f, 0.02f, 0.02f)), glm::vec3(20, i * 20, 0)));
		}
		m_meshes[i].Render(&rootNode);
	}
}

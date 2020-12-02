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
#include "Animation.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AnimationDataStructures.h"
#define M_PI   3.14159265358979323846264338327950288


Model::Model()
{

}

Model::~Model()
{
	m_meshes.clear();
	boneMap.clear();

	//delete(rootNode);
}

void Model::Initialize(ObjectManager* objectManager, glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset, std::string& modelPath, std::string& materialPath)
{
	animationIndex = 0;
	m_textureManager = objectManager->textureManager;
	m_objectManager = objectManager;

	m_modelData = objectManager->modelManager->LoadModelData(modelPath);

	//If model's vertex data was already loaded in another model
	if(m_modelData->m_meshData.size() != 0)
	{
		for (unsigned int i = 0; i < m_modelData->m_meshData.size(); i++)
		{
			//if (m_meshes.size() < m_modelData->m_meshData.size())
			m_meshes.push_back(Mesh(objectManager, materialPath, this, &m_modelData->m_meshData[i]));
			m_meshes[m_meshes.size() - 1].SetTransform(m_modelData->m_meshData[i].meshTransform);
			//TODO: Load nodes from loaded model?

		}

		//Load bones from previously loaded model data
		for (auto it : m_modelData->boneMap)
		{
			boneMap.insert(std::pair<std::string, BoneData>(it));
		}
	}
	//If the model data isn't currently loaded in memory
	else
	{
		LoadModel(modelPath, materialPath);
		//m_modelData->boneMap.insert(boneMap.begin(), boneMap.end());
		for (auto it : boneMap)
		{
			//when copying boneMap data into m_modelData->boneMap, make sure the values of each bone (e.g. offsetTransform) are initialized 
			//with correct values
			m_modelData->boneMap.insert(std::pair<std::string, BoneData>(it));
		}
	}
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

	if (!m_modelData->rootNode)
		m_modelData->rootNode = new Node();
	//m_meshes.resize(scene->mNumMeshes);
	//process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, materialPath, m_modelData->rootNode, nullptr);

	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		Animation newAnim = Animation();
		newAnim.Initialize(scene, i, &m_modelData->boneKeyMap);
		//std::pair<std::string, Animation> newAnimPair(aiScene->mAnimations[i]->mName.C_Str(), newAnim);
		//animationMap.insert(newAnimPair);
		m_modelData->animations.push_back(newAnim);
	}
}

void Model::ProcessNode(aiNode * node, const aiScene * scene, std::string materialPath, Node* currentNode, Node* parentNode)
{
	currentNode->name = node->mName.C_Str();

	currentNode->transform = AiMat4ToGlmMat4(node->mTransformation);
	
	//currentNode->transform = glm::inverse(currentNode->transform);
	//if (parentNode != nullptr)
	//	currentNode->transform = parentNode->transform * currentNode->transform;
	currentNode->parent = parentNode;
	//process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//the node object only contains indices to index the actual objects in the scene.
		//the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, materialPath, node));
		m_meshes[m_meshes.size() - 1].SetTransform(currentNode->transform);
		m_modelData->m_meshData[m_meshes.size() - 1].meshTransform = m_meshes[m_meshes.size() - 1].GetOffsetTransform();
	}

	//after we've processed all of the meshes (if any) when recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		currentNode->children.push_back(Node());
		ProcessNode(node->mChildren[i], scene, materialPath, &currentNode->children[i], currentNode);
	}
}

Mesh Model::ProcessMesh(aiMesh * mesh, std::string& materialPath, const aiNode* node)
{
	if (m_meshes.size() == m_modelData->m_meshData.size())
		m_modelData->m_meshData.push_back(MeshData());
	return Mesh(m_objectManager, mesh, materialPath, this, &(m_modelData->m_meshData[m_meshes.size()]), node, boneMap, totalBones);
}

void Model::SetIsActive(bool newIsActive)
{
	isActive = newIsActive;
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
	WorldComponent::Update(gameTime);
	if(animationIndex < m_modelData->animations.size())
		m_modelData->animations[animationIndex].animationTime += m_modelData->animations[animationIndex].ticksPerSecond * gameTime;
}

void Model::Render(LightManager* lightManager)
{
	if (!isActive)
		return;
	//TODO: Check why the first condition (i < 1) is here
	for (unsigned int i = 0; i < 1 && i < lightManager->TotalLights(); i++)
	{
		std::string lightShaderHandle = std::string("pointLights[") + std::to_string(i) + std::string("].");
		m_meshes[0].shader->SetShaderUniform_vec3((std::string(lightShaderHandle + "position")).c_str(), lightManager->GetLight(i)->componentParent->GetTranslationReference());
		m_meshes[0].shader->SetShaderUniform_vec3((std::string(lightShaderHandle + "ambient")).c_str(), lightManager->GetLight(i)->GetAmbientReference());
		m_meshes[0].shader->SetShaderUniform_vec3((std::string(lightShaderHandle + "diffuse")).c_str(), lightManager->GetLight(i)->GetDiffuseReference());
		m_meshes[0].shader->SetShaderUniform_vec3((std::string(lightShaderHandle + "specular")).c_str(), lightManager->GetLight(i)->GetSpecularReference());
		m_meshes[0].shader->SetShaderUniform_vec1((std::string(lightShaderHandle + "constant")).c_str(), lightManager->GetLight(i)->GetConstantReference());
		m_meshes[0].shader->SetShaderUniform_vec1((std::string(lightShaderHandle + "linear")).c_str(), lightManager->GetLight(i)->GetLinearReference());
		m_meshes[0].shader->SetShaderUniform_vec1((std::string(lightShaderHandle + "quadratic")).c_str(), lightManager->GetLight(i)->GetQuadraticReference());

	}

		//TODO: Potentially move to load function?
	if (boneMap.size() == 0)
		m_meshes[0].shader->SetShaderUniform_mat4fv((char*)"gBones[0]", glm::mat4(1), GL_FALSE);

	if (m_modelData->animations.size() > 0)
		m_modelData->animations[animationIndex].ReadNodeHierarchy(m_modelData->rootNode, glm::mat4(1), boneMap);
		//m_modelData->animations[animationIndex].ReadNodeHierarchy(m_modelData->rootNode, m_modelData->rootNode->transform, boneMap);

	for (auto it : boneMap)
	{
		std::string boneUniform = "gBones[" + std::to_string(it.second.boneID) + "]";
		m_meshes[0].shader->SetShaderUniform_mat4fv((char*)boneUniform.c_str(), boneMap[it.first].finalTransformation, GL_FALSE);
	}

	for (Mesh mesh : m_meshes)
	{
		mesh.shader->SetShaderUniform_mat4fv((char*)"view", m_objectManager->cameraManager->GetCamera(0)->cameraViewMatrix, GL_FALSE);
		mesh.shader->SetShaderUniform_mat4fv((char*)"projection", m_objectManager->cameraManager->GetCamera(0)->projectionMatrix, GL_FALSE);
		if (componentParent != nullptr)
		{
			mesh.shader->SetShaderUniform_mat4fv((char*)"model", componentParent->GetTransform() * offsetTransform * (mesh.GetOffsetTransform()), GL_FALSE);
			//mesh.shader->SetShaderUniform_mat4fv((char*)"model", componentParent->GetTransform() * offsetTransform, GL_FALSE);
		}
		else
		{
			mesh.shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform * mesh.GetOffsetTransform(), GL_FALSE);
		}
		mesh.Render();
	}
}

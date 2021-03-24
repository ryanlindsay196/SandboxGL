#include "Model.h"
#include "Shader.h"
#include <glad/glad.h>
#include "ManagerClasses/TextureManager.h"
//#include "ManagerClasses/ObjectManager.h"
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

Model::~Model()
{
	//Don't delete vertex data, as it's stored in a common area among models
	//Vertex data is deleted when there are no references to it
	m_meshes.clear();
	boneMap.clear();
}

//Initialize variables and load model vertex data
void Model::Initialize(glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset, std::string& modelPath, std::string& materialPath)
{
	animationIndex = 0;
	m_textureManager = TextureManager::GetInstance();

	m_modelData = ModelManager::GetInstance()->LoadModelData(modelPath);

	//If model's vertex data was already loaded in another model
	if(m_modelData->m_meshData.size() != 0)
	{
		for (unsigned int i = 0; i < m_modelData->m_meshData.size(); i++)
		{
			m_meshes.push_back(Mesh(materialPath, this, &m_modelData->m_meshData[i]));
			m_meshes[m_meshes.size() - 1].SetTransform(m_modelData->m_meshData[i].meshTransform);
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
		for (auto it : boneMap)
		{
			m_modelData->boneMap.insert(std::pair<std::string, BoneData>(it));
		}
	}
	for(unsigned int i = 0; i < m_meshes.size(); i++)
		m_meshes[i].AttachMeshData(&m_modelData->m_meshData[i]);

	//Initialize transformation variables
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

//Interface with ASSIMP library to get model data, which are loaded into internal data structures
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
	//process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, materialPath, m_modelData->rootNode, nullptr);

	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		//TODO: Remove the animation copy
		m_modelData->animations.push_back(Animation());
		Animation& newAnim = m_modelData->animations[i];
		newAnim.Initialize(scene, i, &m_modelData->boneKeyMap);
	}
}

//Process each aiNode from ASSIMP recursively, and copy relevant data to the currentNode variable
void Model::ProcessNode(aiNode * node, const aiScene * scene, std::string materialPath, Node* currentNode, Node* parentNode)
{
	currentNode->name = node->mName.C_Str();

	currentNode->transform = AiMat4ToGlmMat4(node->mTransformation);
	
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

	//after processing all of the meshes for this node (if any), recursively process each of the children nodes/meshes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		currentNode->children.push_back(Node());
		ProcessNode(node->mChildren[i], scene, materialPath, &currentNode->children[i], currentNode);
	}
}

//Load vertex data for a new mesh
Mesh Model::ProcessMesh(aiMesh * mesh, std::string& materialPath, const aiNode* node)
{
	//This is true if the model isn't loaded yet
	if (m_meshes.size() == m_modelData->m_meshData.size())
		m_modelData->m_meshData.push_back(MeshData());
	//Create and return a new mesh
	return Mesh(mesh, materialPath, this, &(m_modelData->m_meshData[m_meshes.size()]), node, boneMap, totalBones);
}

//Set the internal isActive variable, which enables/disables rendering for the model
void Model::SetIsActive(bool newIsActive)
{
	isActive = newIsActive;
}

//Load a shader for each mesh
void Model::LoadShaders()
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].LoadShaders();
	}
}

//Get the number of meshes referenced by this model
unsigned int Model::GetLoadedMeshesCount()
{
	return m_meshes.size();
}

//Get a mesh at index i
//Returns nullptr if i is greater than the size of the meshes vector
Mesh * Model::GetMesh(unsigned int i)
{
	if (i < m_meshes.size())
		return &m_meshes[i];
	return nullptr;
}

//Updates the model and animation
void Model::Update(float gameTime)
{
	WorldComponent::Update(gameTime);
	if(animationIndex < m_modelData->animations.size())
		animationTime += m_modelData->animations[animationIndex].ticksPerSecond * gameTime;
}

//Renders the model
void Model::Render(LightManager* lightManager)
{
	//Don't draw if the model component isn't active
	if (!isActive)
		return;

	//Find all lights, and send their positions to the shader
	for (unsigned int i = 0; i < lightManager->TotalLights(); i++)
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

	//If there are no bones, set gBones[0] to identity matrix so the model renders
	if (boneMap.size() == 0)
		m_meshes[0].shader->SetShaderUniform_mat4fv((char*)"gBones[0]", glm::mat4(1), GL_FALSE);

	if (animationTime > 160)
		animationTime = 0;

	if (m_modelData->animations.size() > 0)
	//Calculate bone transformations recursively, for the current animation frame.
		m_modelData->animations[animationIndex].ReadNodeHierarchy(m_modelData->rootNode, glm::mat4(1), boneMap, animationTime);

	//Set all bones in the shader to their appropriate transformations
	for (auto it : boneMap)
	{
		std::string boneUniform = "gBones[" + std::to_string(it.second.boneID) + "]";
		m_meshes[0].shader->SetShaderUniform_mat4fv((char*)boneUniform.c_str(), boneMap[it.first].finalTransformation, GL_FALSE);
	}

	//Set the mvp matrix in the shader
	for (Mesh mesh : m_meshes)
	{
		mesh.shader->SetShaderUniform_mat4fv((char*)"view", CameraManager::GetInstance()->GetCamera(0)->cameraViewMatrix, GL_FALSE);
		mesh.shader->SetShaderUniform_mat4fv((char*)"projection", CameraManager::GetInstance()->GetCamera(0)->projectionMatrix, GL_FALSE);
		//If the model has a parent entity
		if (componentParent != nullptr)
		{
			//Get the models offset from it's parent entity
			mesh.shader->SetShaderUniform_mat4fv((char*)"model", componentParent->GetTransform() * offsetTransform * (mesh.GetOffsetTransform()), GL_FALSE);
		}
		else
		{
			//Set the model's world space transform to its offset transform
			mesh.shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform * mesh.GetOffsetTransform(), GL_FALSE);
		}
		mesh.Render();
	}
}

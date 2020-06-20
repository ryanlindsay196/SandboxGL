#include "ModelManager.h"
#include "TextureManager.h"
#include "ObjectManager.h"
#include <unordered_map>

ModelManager::ModelManager()
{
	//m_textureManager = new TextureManager();
}

ModelManager::~ModelManager()
{
	for (unsigned int i = 0; i < models.size(); i++)
		delete models[i];
	delete(m_textureManager);
}

void ModelManager::Initialize(ObjectManager * objectManager)
{
	m_objectManager = objectManager;
	m_textureManager = objectManager->textureManager;
}

Model* ModelManager::LoadModel(glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset)
{//TODO: Delete cause this is depreciated
	LoadModel((char*)"Resources/Models/guitar_backpack/Rock_Low.fbx", (char*)"Resources/Materials/DefaultMaterial.mat", positionOffset, rotationAxis, rotationAngle, scaleOffset);
	//models.push_back(new Model());
	//models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, (char*)"Resources/Models/guitar_backpack/arms.fbx", (char*)"");
	//models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, (char*)"Resources/Models/guitar_backpack/Survival_BackPack_2.fbx", (char*)"");
	//models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, (char*)"Resources/Models/guitar_backpack/Rock_Low.fbx", (char*)"");
	//models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, (char*)"Resources/Models/guitar_backpack/plane.fbx", (char*)"");
	//models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, (char*)"Resources/Models/guitar_backpack/Altar.fbx", (char*)"");

	return models[models.size() - 1];
}

Model* ModelManager::LoadModel(char * modelPath, std::string materialPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset)
{
	models.push_back(new Model());
	models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, modelPath, (char*)materialPath.c_str());

	modelDataMap.
	return models[models.size() - 1];
}

void ModelManager::LoadShaders()
{
	for (Model* model : models)
	{
		model->LoadShaders();
	}
}

void ModelManager::UpdateModels(float gameTime)
{
	for (Model* model : models)
		model->Update(gameTime);
}

void ModelManager::RenderModels()
{
	for (int i = 0; i < (int)models.size(); i++)
	{
		models[i]->Render();
	}
}

Model * ModelManager::GetModel(int index)
{
	return models[index];
}

unsigned int ModelManager::LoadedModelsCount()
{
	return models.size();
}

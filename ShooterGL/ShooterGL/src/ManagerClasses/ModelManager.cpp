#include "ModelManager.h"
#include "TextureManager.h"
#include "ObjectManager.h"

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
{//Loads default model (triangle/rectangle)
	models.push_back(new Model());
	models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, (char*)"", (char*)"");

	return models[models.size() - 1];
}

Model* ModelManager::LoadModel(char * modelPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset)
{
	models.push_back(new Model());
	models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, modelPath, (char*)"");

	return models[models.size() - 1];
}

Model* ModelManager::LoadModel(char * modelPath, char * texturePath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset)
{//TODO: Check for depreciation because the textures will be on the shader/materials
	return LoadModel(modelPath, positionOffset, rotationAxis, rotationAngle, scaleOffset);
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

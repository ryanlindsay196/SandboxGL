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

Model* ModelManager::LoadModel()
{//Loads default model (triangle/rectangle)
	models.push_back(new Model());
	models[models.size() - 1]->Initialize(m_objectManager);

	return models[models.size() - 1];
}

Model* ModelManager::LoadModel(char * modelPath)
{
	models.push_back(new Model());
	models[models.size() - 1]->Initialize(m_objectManager);

	return models[models.size() - 1];
}

Model* ModelManager::LoadModel(char * modelPath, char * texturePath)
{//TODO: Check for depreciation
	return LoadModel(modelPath);
}

void ModelManager::UpdateModels()
{
	for (Model* model : models)
		model->Update();
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

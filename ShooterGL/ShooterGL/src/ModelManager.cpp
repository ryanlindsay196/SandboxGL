#include "ModelManager.h"

void ModelManager::LoadModel()
{
	models.push_back(new Model());
	models[models.size() - 1]->Initialize();
}

void ModelManager::LoadModel(char * modelPath)
{
	models.push_back(new Model());
	models[models.size() - 1]->Initialize();
}

void ModelManager::LoadModel(char * modelPath, char * texturePath)
{
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

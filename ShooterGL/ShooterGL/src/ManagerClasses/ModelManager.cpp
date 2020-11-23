#include "ModelManager.h"
#include "TextureManager.h"
#include "ObjectManager.h"
#include "ModelData.h"
#include <unordered_map>

#include <cctype>
#include <algorithm>

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
	for (unsigned int i = 0; i < models.size(); i++)
		delete models[i];
}

void ModelManager::Initialize(ObjectManager * objectManager)
{
	for (unsigned int i = 0; i < models.size(); i++)
		delete models[i];
	models.clear();
	m_objectManager = objectManager;
	m_textureManager = objectManager->textureManager;
}

Model* ModelManager::LoadModel(std::string& modelPath, std::string materialPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset)
{
	models.push_back(new Model());
	models[models.size() - 1]->Initialize(m_objectManager, positionOffset, rotationAxis, rotationAngle, scaleOffset, modelPath, materialPath);

	return models[models.size() - 1];
}

std::shared_ptr<ModelData> ModelManager::LoadModelData(std::string hashID)
{
	//Make the hashID lower case
	std::transform(hashID.begin(), hashID.end(), hashID.begin(),
		[](unsigned char c) { return std::tolower(c); });
	auto modelData = modelDataMap.find(hashID);
	if (modelData == modelDataMap.end())
	{
		std::shared_ptr<ModelData> newModelData = std::make_shared<ModelData>();

		std::pair<std::string, std::shared_ptr<ModelData>> newModelDataEntry(hashID, newModelData);
		modelDataMap.insert(newModelDataEntry);

		return newModelDataEntry.second;
	}
	return modelDataMap[hashID];
}

void ModelManager::LoadShaders()
{
	for (Model* model : models)
	{
		model->LoadShaders();
	}
}

void ModelManager::UnloadModelData()
{
	auto it = modelDataMap.cbegin();
	while (it != modelDataMap.cend())
	{
		if (modelDataMap.find(it._Ptr->_Myval.first) != modelDataMap.cend())
		{
			if (it._Ptr->_Myval.second.use_count() == 1l)
			{
				it = modelDataMap.erase(it);
			}
			else
				it++;
		}
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

#include "ModelManager.h"
#include "TextureManager.h"
//#include "ObjectManager.h"
#include "ModelData.h"
#include <unordered_map>

#include <cctype>
#include <algorithm>

ModelManager* ModelManager::instance = 0;

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
	models.clear();
	//for (unsigned int i = 0; i < models.size(); i++)
	//	delete models[i];
}

ModelManager * ModelManager::GetInstance()
{
	if (!instance)
		instance = new ModelManager();
	return instance;
}

void ModelManager::Initialize()
{
	//for (unsigned int i = 0; i < models.size(); i++)
	//	delete models[i];
	models.clear();
	m_textureManager = TextureManager::GetInstance();
}

Model* ModelManager::LoadModel(std::string& modelPath, std::string materialPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset, unsigned int newEntityComponentIndex, Entity* parentEntity)
{
	//models.push_back(new Model());
	AddComponent(models, newEntityComponentIndex, parentEntity);
	models[models.size() - 1].Initialize(positionOffset, rotationAxis, rotationAngle, scaleOffset, modelPath, materialPath);

	return &models[models.size() - 1];
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
	for (unsigned int i = 0; i < models.size(); i++)
	{
		models[i].LoadShaders();
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
	return &models[index];
}

unsigned int ModelManager::LoadedModelsCount()
{
	return models.size();
}

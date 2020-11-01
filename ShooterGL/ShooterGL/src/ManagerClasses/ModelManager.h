#pragma once

#include <vector>
#include "Renderables/Model.h"
#include <unordered_map>
#include <memory>

class TextureManager;
class ObjectManager;

class ModelManager
{
private:
	std::vector<Model*> models;
	std::unordered_map<std::string, std::shared_ptr<ModelData>> modelDataMap;

	TextureManager* m_textureManager;
	ObjectManager* m_objectManager;

public:
	ModelManager();
	~ModelManager();

	void Initialize(ObjectManager * objectManager);

	Model* LoadModel(char* modelPath, std::string materialPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset);
	std::shared_ptr<ModelData> LoadModelData(std::string hashID);
	void LoadShaders();

	void UnloadModelData();

	Model* GetModel(int index);

	unsigned int LoadedModelsCount();
};
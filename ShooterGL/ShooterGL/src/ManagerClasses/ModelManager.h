#pragma once

#include <vector>
#include "Renderables/Model.h"
#include <unordered_map>

class TextureManager;
class ObjectManager;

class ModelManager
{
private:
	std::vector<Model*> models;
	std::unordered_map<std::string, ModelData*> modelDataMap;

	TextureManager* m_textureManager;
	ObjectManager* m_objectManager;

public:
	ModelManager();
	~ModelManager();

	void Initialize(ObjectManager * objectManager);

	Model* LoadModel(char* modelPath, std::string materialPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset);
	ModelData* LoadModelData(std::string hashID);
	void LoadShaders();

	Model* GetModel(int index);

	unsigned int LoadedModelsCount();
};
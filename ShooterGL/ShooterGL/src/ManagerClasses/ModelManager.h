#pragma once

#include <vector>
#include "Renderables/Model.h"
#include <unordered_map>
#include <memory>
#include "BaseManager.h"

class TextureManager;
class ObjectManager;

class ModelManager : public BaseManager
{
private:
	std::vector<Model> models;
	std::unordered_map<std::string, std::shared_ptr<ModelData>> modelDataMap;

	TextureManager* m_textureManager;

	static ModelManager* instance;
	ModelManager();
public:
	~ModelManager();

	static ModelManager* GetInstance();

	void Initialize();

	Model* LoadModel(std::string& modelPath, std::string materialPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset, unsigned int newEntityComponentIndex, Entity* parentEntity);
	std::shared_ptr<ModelData> LoadModelData(std::string hashID);
	void LoadShaders();

	void UnloadModelData();

	Model* GetModel(int index);

	unsigned int LoadedModelsCount();
};
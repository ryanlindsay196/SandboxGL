#pragma once

#include <vector>
#include "Renderables/Model.h"

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

	Model* LoadModel(glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset);
	Model* LoadModel(char* modelPath, std::string materialPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset);
	void LoadShaders();

	void UpdateModels(float gameTime);

	void RenderModels();

	Model* GetModel(int index);

	unsigned int LoadedModelsCount();
};
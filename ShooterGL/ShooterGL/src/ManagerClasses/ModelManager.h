#pragma once

#include <vector>
#include "Renderables/Model.h"

class TextureManager;
class ObjectManager;

class ModelManager
{
private:
	std::vector<Model*> models;

	TextureManager* m_textureManager;
	ObjectManager* m_objectManager;

public:
	ModelManager();
	~ModelManager();

	void Initialize(ObjectManager * objectManager);

	Model* LoadModel();
	Model* LoadModel(char* modelPath);
	Model* LoadModel(char* modelPath, char* texturePath);

	void UpdateModels();

	void RenderModels();

	Model* GetModel(int index);
};
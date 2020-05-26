#pragma once

#include <vector>
#include "Renderables/Model.h"

class TextureManager;

class ModelManager
{
private:
	std::vector<Model*> models;

	TextureManager* m_textureManager;

public:
	ModelManager();
	~ModelManager();

	void LoadModel();
	void LoadModel(char* modelPath);
	void LoadModel(char* modelPath, char* texturePath);

	void UpdateModels();

	void RenderModels();
};
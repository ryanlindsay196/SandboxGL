#pragma once

#include <vector>
#include "Model.h"

class ModelManager
{
private:
	std::vector<Model*> models;

public:
	void LoadModel();
	void LoadModel(char* modelPath);
	void LoadModel(char* modelPath, char* texturePath);

	void UpdateModels();

	void RenderModels();
};
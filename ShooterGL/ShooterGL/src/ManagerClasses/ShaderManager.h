#pragma once
#include <unordered_map>
#include "ObjectManager.h"

class Shader;

class ShaderManager {
private:
	std::unordered_map<std::string, Shader*> shaders;
	//std::unordered_map<std::string, int> shaderInstances;
public:
	Shader* LoadNewShader(std::string materialPath, ObjectManager* in_objectManager);
};

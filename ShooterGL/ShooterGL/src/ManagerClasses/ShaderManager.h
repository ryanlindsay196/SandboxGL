#pragma once
#include <unordered_map>
#include "ObjectManager.h"
#include <memory>

class Shader;

class ShaderManager {
private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
	//std::unordered_map<std::string, int> shaderInstances;
public:
	std::shared_ptr<Shader> LoadNewShader(std::string materialPath, ObjectManager* in_objectManager);
	void UnloadShaders();
};

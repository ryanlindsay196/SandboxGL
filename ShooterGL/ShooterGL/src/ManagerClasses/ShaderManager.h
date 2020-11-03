#pragma once
#include <unordered_map>
#include "ObjectManager.h"
#include <memory>

class Shader;

class ShaderManager {
private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
public:
	std::shared_ptr<Shader> LoadNewShader(std::string materialPath, ObjectManager* in_objectManager);
	void UnloadShaders();
};

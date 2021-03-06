#pragma once
#include <unordered_map>
//#include "ObjectManager.h"
#include <memory>

class Shader;

class ShaderManager {
private:
	ShaderManager();
	static ShaderManager* instance;

	std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
public:
	static ShaderManager* GetInstance();

	std::shared_ptr<Shader> LoadNewShader(std::string materialPath);
	void UnloadShaders();
};

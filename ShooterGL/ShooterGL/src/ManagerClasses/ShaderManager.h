#pragma once
#include <unordered_map>
#include <memory>
#include "BaseManager.h"

class Shader;

class ShaderManager : public BaseManager
{
private:
	ShaderManager();
	ShaderManager(const ShaderManager&) = delete;
	static ShaderManager* instance;

	std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
public:
	static ShaderManager* GetInstance();

	std::shared_ptr<Shader> LoadNewShader(std::string materialPath);
	void UnloadShaders();
};

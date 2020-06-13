#include "ShaderManager.h"
#include "../Renderables/Shader.h"
#include "TextureManager.h"
#include "ObjectManager.h"

Shader* ShaderManager::LoadNewShader(std::string materialPath, ObjectManager* in_objectManager)
{
	auto shader = shaders.find(materialPath);
	if (shader == shaders.end())
	{
		Shader* newShader = new Shader();
		newShader->Initialize(in_objectManager, (char*)"", (char*)"", (char*)materialPath.c_str());

		std::pair<std::string, Shader*> newShaderEntry(materialPath, newShader);
		shaders.insert(newShaderEntry);
		return newShaderEntry.second;// shaders.find(shaderPath);
	}
	for (auto& x : shaders)
	{//TODO: Replace this with instant lookup
		if (x.first == materialPath)
			return x.second;
	}
	return nullptr;
}

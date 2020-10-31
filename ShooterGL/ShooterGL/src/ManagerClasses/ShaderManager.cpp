#include "ShaderManager.h"
#include "../Renderables/Shader.h"
#include "TextureManager.h"
#include "ObjectManager.h"

std::shared_ptr<Shader> ShaderManager::LoadNewShader(std::string materialPath, ObjectManager* in_objectManager)
{
	auto shader = shaders.find(materialPath);
	if (shader == shaders.end())
	{
		std::shared_ptr<Shader> newShader = std::make_shared<Shader>();
		newShader->Initialize(in_objectManager, (char*)"", (char*)"", (char*)materialPath.c_str());

		std::pair<std::string, std::weak_ptr<Shader>> newShaderEntry(materialPath, newShader);
		shaders.insert(newShaderEntry);

		return shaders[newShaderEntry.first];
	}
	return shaders[materialPath];
	return nullptr;
}

void ShaderManager::UnloadShaders()
{
	for (auto shader : shaders)
	{
		if (shader.second.use_count() == 1l)
		{
			shaders.erase(shaders.find(shader.first));
		}
	}
}

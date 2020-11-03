#include "ShaderManager.h"
#include "../Renderables/Shader.h"
#include "TextureManager.h"
#include "ObjectManager.h"

#include <cctype>
#include <algorithm>

std::shared_ptr<Shader> ShaderManager::LoadNewShader(std::string materialPath, ObjectManager* in_objectManager)
{
	std::transform(materialPath.begin(), materialPath.end(), materialPath.begin(),
		[](unsigned char c) { return std::tolower(c); });
	auto shader = shaders.find(materialPath);
	if (shader == shaders.end())
	{
		std::shared_ptr<Shader> newShader = std::make_shared<Shader>();
		std::string vertexPath = "";
		std::string fragmentPath = "";
		newShader->Initialize(in_objectManager, vertexPath, fragmentPath, materialPath);

		std::pair<std::string, std::weak_ptr<Shader>> newShaderEntry(materialPath, newShader);
		shaders.insert(newShaderEntry);

		return shaders[newShaderEntry.first];
	}
	return shaders[materialPath];
	return nullptr;
}

void ShaderManager::UnloadShaders()
{
	auto it = shaders.cbegin();
	while (it != shaders.cend())
	{
		if (shaders.find(it._Ptr->_Myval.first) != shaders.cend())
		{
			if (it._Ptr->_Myval.second.use_count() == 1l)
			{
				it = shaders.erase(it);
			}
			else
				it++;
		}
	}
}

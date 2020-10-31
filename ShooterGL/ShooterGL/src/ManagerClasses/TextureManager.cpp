#include "TextureManager.h"
#include "Renderables/Texture.h"

TextureManager::TextureManager()
{
	//textures = std::unordered_map<std::string, Texture*>();
}

std::shared_ptr<Texture> TextureManager::LoadNewTexture(std::string texturePath)
{
	auto texture = textures.find(texturePath);
	if (texture == textures.end())
	{
		std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();
		newTexture->LoadTexture((char*)texturePath.c_str());

		std::pair<std::string, std::weak_ptr<Texture>> newTextureEntry(texturePath, newTexture);
		textures.insert(newTextureEntry);
		return textures[newTextureEntry.first];
	}
	return textures[texturePath];
	return nullptr;
}

void TextureManager::UnloadTextures()
{
	for (auto texture : textures)
	{
		if (texture.second.use_count() == 1l)
		{
			textures.erase(textures.find(texture.first));
		}
	}
}

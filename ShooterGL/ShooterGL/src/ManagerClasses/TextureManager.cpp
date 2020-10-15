#include "TextureManager.h"
#include "Renderables/Texture.h"

TextureManager::TextureManager()
{
	textures = std::unordered_map<std::string, Texture*>();
}

Texture * TextureManager::LoadNewTexture(std::string texturePath)
{
	auto texture = textures.find(texturePath);
	if (texture == textures.end())
	{
		Texture* newTexture = new Texture();
		newTexture->LoadTexture((char*)texturePath.c_str());

		std::pair<std::string, Texture*> newTextureEntry(texturePath, newTexture);
		textures.insert(newTextureEntry);
		return newTextureEntry.second;// textures.find(texturePath);
	}
	//for (auto& x : textures)
	//{//TODO: Replace this with instant lookup
	//	if (x.first == texturePath)
	//		return x.second;
	//}
	return textures[texturePath];
	return nullptr;
}

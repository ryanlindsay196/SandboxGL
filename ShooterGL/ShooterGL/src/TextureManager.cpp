#include "TextureManager.h"
#include "Texture.h"

Texture * TextureManager::LoadNewTexture(char * texturePath)
{
	auto texture = textures.find(texturePath);
	if (texture == textures.end())
	{
		Texture* newTexture = new Texture();
		newTexture->LoadTexture(texturePath);

		std::pair<char*, Texture*> newTextureEntry(texturePath, newTexture);
		//textures.insert(std::make_pair<char*, Texture*>(texturePath, newTexture));
		textures.insert(newTextureEntry);
		return newTextureEntry.second;// textures.find(texturePath);
	}
	//auto& returnTexture = std::pair<void*, void*>(nullptr, nullptr);
	for (auto& x : textures)
	{//TODO: Replace this with instant lookup
		if (x.first == texturePath)
			return x.second;
	}
	return nullptr;
}

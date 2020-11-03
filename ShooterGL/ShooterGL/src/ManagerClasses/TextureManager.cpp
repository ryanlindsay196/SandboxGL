#include "TextureManager.h"
#include "Renderables/Texture.h"

#include <cctype>
#include <algorithm>

TextureManager::TextureManager()
{
}

std::shared_ptr<Texture> TextureManager::LoadNewTexture(std::string texturePath)
{
	std::transform(texturePath.begin(), texturePath.end(), texturePath.begin(),
		[](unsigned char c) { return std::tolower(c); });
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
	auto it = textures.cbegin();
	while (it != textures.cend())
	{
		if (textures.find(it._Ptr->_Myval.first) != textures.cend())
		{
			if (it._Ptr->_Myval.second.use_count() == 1l)
			{
				it = textures.erase(it);
			}
			else
				it++;
		}
	}
}

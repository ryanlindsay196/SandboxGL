#pragma once
#include <unordered_map>

class Texture;
class TextureManager
{
private:
	std::unordered_map<std::string, Texture*> textures;
public:
	TextureManager();
	Texture* LoadNewTexture(std::string texturePath);
};
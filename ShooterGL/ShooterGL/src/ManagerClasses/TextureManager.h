#pragma once
#include <unordered_map>

class Texture;
class TextureManager
{
private:
	std::unordered_map<char*, Texture*> textures;
public:
	TextureManager();
	Texture* LoadNewTexture(char* texturePath);
};
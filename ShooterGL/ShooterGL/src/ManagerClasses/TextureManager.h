#pragma once
#include <unordered_map>

class Texture;
class TextureManager
{
private:
	std::unordered_map<char*, Texture*> textures;
public:
	Texture* LoadNewTexture(char* texturePath);
};
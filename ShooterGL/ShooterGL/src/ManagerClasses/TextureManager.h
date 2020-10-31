#pragma once
#include <unordered_map>
#include <memory>

class Texture;
class TextureManager
{
private:
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
public:
	TextureManager();
	std::shared_ptr<Texture> LoadNewTexture(std::string texturePath);
	void UnloadTextures();
};
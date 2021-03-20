#pragma once
#include <unordered_map>
#include <memory>
#include "BaseManager.h"

class Texture;
class TextureManager : public BaseManager
{
private:
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
	TextureManager();
	static TextureManager* instance;
public:
	static TextureManager* GetInstance();
	std::shared_ptr<Texture> LoadNewTexture(std::string texturePath);
	void UnloadTextures();
};
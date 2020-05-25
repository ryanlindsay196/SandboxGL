#pragma once
//class GLenum;
//class GLint;
class Texture
{
public:
	Texture();
	~Texture();

	void LoadTexture(char* texturePath);
	void UseTexture(int textureSlot);
	void SetDefaultTextureParameters();
	void SetTextureParameters(unsigned int textureDimensions, unsigned int textureAxis, unsigned int textureWrapMode);
	unsigned int GetTextureID();

private:
	int width, height, nrChannels;
	unsigned int textureID;
};
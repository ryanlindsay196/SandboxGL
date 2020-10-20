#include "Texture.h"
#include "glad/glad.h"
#include "stb_image.h"
#include "iostream"

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::LoadTexture(char *texturePath)
{
	SetDefaultTextureParameters();
	unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	glGenTextures(1, &textureID);

	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		std::string extension = texturePath;
		extension = extension.substr(extension.length() - 4);
		if(extension == ".jpg")
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(extension == ".png")
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << texturePath << std::endl;
	}

	stbi_image_free(data);
}

void Texture::UseTexture(int textureSlot)
{
	glActiveTexture(GL_TEXTURE0 + textureSlot); 
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::SetDefaultTextureParameters()
{
	SetTextureParameters(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	SetTextureParameters(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	SetTextureParameters(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	SetTextureParameters(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SetTextureParameters(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	SetTextureParameters(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glGenerateMipmaps(GL_TEXTURE_2D);
}

void Texture::SetTextureParameters(unsigned int textureDimensions, unsigned int textureAxis, unsigned int textureWrapMode)
{
	glTexParameteri(textureDimensions, textureAxis, textureWrapMode);
}

unsigned int Texture::GetTextureID()
{
	return 0;// textureID - 1;
}

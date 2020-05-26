#pragma once
#include <vector>
#include <glm.hpp>
class Texture;
class TextureManager;

class Shader
{
public:
	void Initialize(TextureManager* textureManager, char* vertexPath, char* fragmentPath, std::vector<char*> texturePaths);
	void BindTextures();
	void SetVertexShader(char* vertexPath);
	void SetFragmentShader(char* fragmentPath);

	unsigned int GetShaderProgram();
	void UseShader();

	int GetShaderUniform_vec1(char* uniformName);
	void SetShaderUniform_vec1(char* uniformName, int uniformValue);
	void SetShaderUniform_vec2(char* uniformName, int x, int y);
	void SetShaderUniform_vec3(char* uniformName, int x, int y, int z);
	void SetShaderUniform_vec4(char* uniformName, int x, int y, int z, int w);

	void SetShaderUniform_veci1(char* uniformName, int uniformValue);
	void SetShaderUniform_veci2(char* uniformName, int x, int y);
	void SetShaderUniform_veci3(char* uniformName, int x, int y, int z);
	void SetShaderUniform_veci4(char* uniformName, int x, int y, int z, int w);

	void SetShaderUniform_mat4fv(char* uniformName, glm::mat4 matrix);

private:
	TextureManager* textureManager;

	unsigned int vertexShader;
	char* vertexShaderSource;

	unsigned int fragmentShader;
	char* fragmentShaderSource;

	unsigned int shaderProgram;

	std::vector<Texture*> textures;
};
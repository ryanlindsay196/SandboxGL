#pragma once
#include <vector>
#include <glm.hpp>
class Texture;
class TextureManager;
class ObjectManager;

typedef unsigned char GLboolean;

class Shader
{
public:
	void Initialize(ObjectManager * in_objectManager, char* vertexPath, char* fragmentPath, char* materialPath);
	void LoadMaterial(char* materialPath);
	void AddNewTexture(char* texturePath, char* textureUniform);
	void ClearTextures();
	void BindTextures();
	void SetVertexShader(char* vertexPath);
	void SetFragmentShader(char* fragmentPath);

	std::vector<Texture*> GetTextures();

	unsigned int GetShaderProgram();
	void UseShader();

	void SetShaderUniform_vec1(char* uniformName, float uniformValue);
	void SetShaderUniform_vec2(char* uniformName, float x, float y);
	void SetShaderUniform_vec3(char* uniformName, float x, float y, float z);
	void SetShaderUniform_vec3(char* uniformName, glm::vec3 &value);
	void SetShaderUniform_vec4(char* uniformName, float x, float y, float z, float w);

	void SetShaderUniform_veci1(char* uniformName, int uniformValue);
	void SetShaderUniform_veci2(char* uniformName, int x, int y);
	void SetShaderUniform_veci3(char* uniformName, int x, int y, int z);
	void SetShaderUniform_veci4(char* uniformName, int x, int y, int z, int w);

	void SetShaderUniform_mat4fv(char* uniformName, glm::mat4 matrix, GLboolean toTranspose);

private:
	TextureManager* textureManager;
	ObjectManager* m_objectManager;

	unsigned int vertexShader;
	char* vertexShaderSource;

	unsigned int fragmentShader;
	char* fragmentShaderSource;

	unsigned int shaderProgram;

	std::vector<Texture*> textures;
	std::vector<std::string> textureUniforms;
};
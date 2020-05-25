#pragma once

class Shader
{
public:
	void Initialize(char* vertexPath, char* fragmentPath);
	void SetVertexShader(char* vertexPath);
	void SetFragmentShader(char* fragmentPath);

	unsigned int GetShaderProgram();

	int GetShaderUniform_vec1(char* uniformName);
	void SetShaderUniform_vec1(char* uniformName, int uniformValue);

private:
	unsigned int vertexShader;
	char* vertexShaderSource;

	unsigned int fragmentShader;
	char* fragmentShaderSource;

	unsigned int shaderProgram;
};
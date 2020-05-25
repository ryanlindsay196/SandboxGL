#pragma once

class Shader;
class TextureManager;

class Model
{
public:
	Model();
	~Model();

	void Initialize(TextureManager* textureManager);
	void SetDefaultShaders();
	void SetShaders(char* vertexPath, char* fragmentPath);
	void SetVertexShader(char* vertexPath);
	void SetFragmentShader(char* fragmentPath);

	void Update();
	void Render();

private:
	unsigned int VBO;//Vertex buffer object
	unsigned int VAO;//Vertex array object - stores settings from VBO
	unsigned int EBO;//Element buffer object (Index buffer) - stores indices of vertices so we only store each vertex once

	TextureManager* m_textureManager;
	Shader* shader;
};


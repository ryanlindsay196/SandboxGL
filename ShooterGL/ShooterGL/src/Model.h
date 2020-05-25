#pragma once

class Shader;
class Model
{
public:
	Model();
	~Model();

	void Initialize();
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

	Shader* shader;
};


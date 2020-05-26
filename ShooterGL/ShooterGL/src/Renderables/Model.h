#pragma once
//#include "Component.h"
#include "../WorldComponent.h"

class Shader;
class ObjectManager;
class TextureManager;

class Model : public WorldComponent
{
public:
	Model();
	~Model();

	void Initialize(ObjectManager* textureManager, glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset);
	void SetDefaultShaders();
	void SetShaders(char* vertexPath, char* fragmentPath);
	void SetVertexShader(char* vertexPath);
	void SetFragmentShader(char* fragmentPath);

	virtual void Update(float gameTime) override;
	void Render();

private:
	unsigned int VBO;//Vertex buffer object
	unsigned int VAO;//Vertex array object - stores settings from VBO
	unsigned int EBO;//Element buffer object (Index buffer) - stores indices of vertices so we only store each vertex once

	ObjectManager* m_objectManager;
	TextureManager* m_textureManager;
	Shader* shader;
};


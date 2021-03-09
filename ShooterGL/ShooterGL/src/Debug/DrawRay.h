#pragma once
#include "glad/glad.h"
#include "glm.hpp"
#include <vector>

//TODO: Delete this?
class DebugRay
{
private:
	static DebugRay* instance;
	DebugRay();
public:
	static DebugRay* GetInstance();
	//static void DrawLine(glm::vec3 startPos, glm::vec3 endPos, const glm::mat4& parentTransform);
};

class Entity;

class DebugLines {
private:
	static DebugLines* instance;
	DebugLines();


	std::vector<Entity*> parentEntities;
	std::vector<float> vertices;

	int shaderProgram;
	unsigned int VBO, VAO;
public:
	static DebugLines* GetInstance();
	static void AddLine(glm::vec3 startPos, glm::vec3 endPos, Entity* parentEntity);
	static void DrawLines();

	static void ClearVertices();
};
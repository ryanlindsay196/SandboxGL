#pragma once
#include "glad/glad.h"
#include "glm.hpp"
#include <vector>

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
	static void AddLine(glm::vec3 startPos, glm::vec3 endPos, Entity* parentEntity, glm::vec3 color);
	static void DrawLines();

	static void ClearVertices();
};
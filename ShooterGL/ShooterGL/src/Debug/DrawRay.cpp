#include "Debug/DrawRay.h"
#include "ManagerClasses/CameraManager.h"
#include "../Entity.h"

#ifdef DEBUG
#include <iostream>
#endif //DEBUG


DebugLines* DebugLines::instance = 0;

DebugLines::DebugLines()
{
}

DebugLines * DebugLines::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new DebugLines();

		const char *vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aColor;\n"
			"uniform mat4 MVP;\n"
			"out vec3 color;\n"
			"void main()\n"
			"{\n"
			"	color = aColor;\n"
			"   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";
		const char *fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"in vec3 color;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(color, 1.0f);\n"
			"}\n\0";

		// vertex shader
		int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		// check for shader compile errors

		// fragment shader
		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		// check for shader compile errors

		// link shaders
		instance->shaderProgram = glCreateProgram();
		glAttachShader(instance->shaderProgram, vertexShader);
		glAttachShader(instance->shaderProgram, fragmentShader);
		glLinkProgram(instance->shaderProgram);
		// check for linking errors


		int success;
		char infoLog[512];
		glGetProgramiv(instance->shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(instance->shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;

		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glGenVertexArrays(1, &instance->VAO);
		glGenBuffers(1, &instance->VBO);
		glBindVertexArray(instance->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, instance->VBO);
		//glBufferData(GL_ARRAY_BUFFER, instance->vertices.size() * sizeof(float), instance->vertices.data(), GL_STATIC_DRAW);

		//Allocate 1000 vertices (500 debug lines) in this buffer
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float) * 1000, nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


	}
	return instance;
}

void DebugLines::AddLine(glm::vec3 startOffsetPos, glm::vec3 endOffsetPos, Entity* parentEntity, glm::vec3 color)
{
	GetInstance();
	instance->vertices.push_back(startOffsetPos.x);
	instance->vertices.push_back(startOffsetPos.y);
	instance->vertices.push_back(startOffsetPos.z);
	instance->vertices.push_back(color.x);
	instance->vertices.push_back(color.y);
	instance->vertices.push_back(color.z);
	instance->vertices.push_back(endOffsetPos.x);
	instance->vertices.push_back(endOffsetPos.y);
	instance->vertices.push_back(endOffsetPos.z);
	instance->vertices.push_back(color.x);
	instance->vertices.push_back(color.y);
	instance->vertices.push_back(color.z);

	instance->parentEntities.push_back(parentEntity);
}

void DebugLines::DrawLines()
{
	if (instance->vertices.size() == 0)
		return;

	for (unsigned int i = 0; i < instance->parentEntities.size(); i++)
	{
		if (instance->parentEntities[i] != nullptr)
		{
			glm::vec4 startVertex(
				instance->vertices[(i * 12) + 0],
				instance->vertices[(i * 12) + 1],
				instance->vertices[(i * 12) + 2],
				1
			);
			glm::vec4 endVertex(
				instance->vertices[(i * 12) + 6],
				instance->vertices[(i * 12) + 7],
				instance->vertices[(i * 12) + 8],
				1
			);
			startVertex = instance->parentEntities[i]->GetTransform() * startVertex;
			endVertex = instance->parentEntities[i]->GetTransform() * endVertex;
			instance->vertices[(i * 12) + 0] = startVertex.x;
			instance->vertices[(i * 12) + 1] = startVertex.y;
			instance->vertices[(i * 12) + 2] = startVertex.z;
			instance->vertices[(i * 12) + 6] = endVertex.x;
			instance->vertices[(i * 12) + 7] = endVertex.y;
			instance->vertices[(i * 12) + 8] = endVertex.z;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, instance->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instance->vertices.size() * sizeof(float), &instance->vertices[0]);

	glm::mat4 MVP = CameraManager::GetInstance()->GetCamera(0)->projectionMatrix * CameraManager::GetInstance()->GetCamera(0)->cameraViewMatrix;
	//glm::vec3 lineColor = glm::vec3(0, 1, 0);

	glUseProgram(instance->shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(instance->shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//glUniform3fv(glGetUniformLocation(instance->shaderProgram, "color"), 1, &lineColor[0]);

	glBindVertexArray(instance->VAO);
	glDrawArrays(GL_LINES, 0, instance->vertices.size());
	glBindVertexArray(0);
}

void DebugLines::ClearVertices()
{
	if (instance != nullptr)
	{
		instance->vertices.clear();
		instance->parentEntities.clear();
	}
}

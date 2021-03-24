#include "Debug/DrawRay.h"
#include "ManagerClasses/CameraManager.h"
#include "../Entity.h"

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
			"uniform mat4 MVP;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";
		const char *fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"uniform vec3 color;\n"
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

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glGenVertexArrays(1, &instance->VAO);
		glGenBuffers(1, &instance->VBO);
		glBindVertexArray(instance->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, instance->VBO);
		//glBufferData(GL_ARRAY_BUFFER, instance->vertices.size() * sizeof(float), instance->vertices.data(), GL_STATIC_DRAW);

		//Allocate 1000 vertices (500 debug lines) in this buffer
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * 1000, nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


	}
	return instance;
}

void DebugLines::AddLine(glm::vec3 startPos, glm::vec3 endPos, Entity* parentEntity)
{
	GetInstance();
	instance->vertices.push_back(startPos.x);
	instance->vertices.push_back(startPos.y);
	instance->vertices.push_back(startPos.z);
	instance->vertices.push_back(endPos.x);
	instance->vertices.push_back(endPos.y);
	instance->vertices.push_back(endPos.z);

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
			instance->vertices[(i * 6) + 0] += instance->parentEntities[i]->GetTranslation().x;
			instance->vertices[(i * 6) + 1] += instance->parentEntities[i]->GetTranslation().y;
			instance->vertices[(i * 6) + 2] += instance->parentEntities[i]->GetTranslation().z;
			instance->vertices[(i * 6) + 3] += instance->parentEntities[i]->GetTranslation().x;
			instance->vertices[(i * 6) + 4] += instance->parentEntities[i]->GetTranslation().y;
			instance->vertices[(i * 6) + 5] += instance->parentEntities[i]->GetTranslation().z;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, instance->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instance->vertices.size() * sizeof(float), &instance->vertices[0]);

	glm::mat4 MVP = CameraManager::GetInstance()->GetCamera(0)->projectionMatrix * CameraManager::GetInstance()->GetCamera(0)->cameraViewMatrix;
	glm::vec3 lineColor = glm::vec3(0, 1, 0);

	glUseProgram(instance->shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(instance->shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(glGetUniformLocation(instance->shaderProgram, "color"), 1, &lineColor[0]);

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

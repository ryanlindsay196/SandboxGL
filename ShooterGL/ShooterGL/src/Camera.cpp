#include "Camera.h"

void Camera::Initialize(Entity * newParent)
{
	componentParent = newParent;
	viewMatrix = glm::mat4(1);

	//viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -1.0f));
	projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.01f, 100.0f);
}

void Camera::Update(float gameTime)
{
	//Translate(glm::vec3(0, 0, 0.0001));
	Rotate(glm::vec3(0, 1, 0), gameTime * 0.01);
}

void Camera::Translate(glm::vec3 moveBy)
{
	viewMatrix = glm::translate(viewMatrix, moveBy);
	//TODO: make this work
}

void Camera::Rotate(glm::vec3 rotationAxis, float rotationAngle)
{
	//TODO: make this work
	viewMatrix = glm::rotate(viewMatrix, rotationAngle, rotationAxis);
}

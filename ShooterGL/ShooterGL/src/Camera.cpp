#include "Camera.h"

void Camera::Initialize()
{
	//cameraViewMatrix = componentParent->GetTransform();

	//viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -1.0f));
	projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.01f, 100.0f);
	WorldComponent::Initialize();
}

void Camera::Update(float gameTime)
{
	cameraDirection = glm::normalize(componentParent->GetDirection());
	glm::vec3 cameraPos = componentParent->GetTranslation();
	cameraRight = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection);
	cameraUp = glm::vec3(0, 1, 0);// glm::cross(cameraDirection, cameraRight);

	cameraViewMatrix = glm::lookAt(cameraPos,
							cameraPos + (cameraDirection * glm::vec3(1,-1,1)),
							cameraUp);
	//cameraViewMatrix = componentParent->GetTransform();
	//cameraViewMatrix = glm::lookAt(glm::vec3(-15, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	WorldComponent::Update(gameTime);
}

glm::vec3 Camera::GetCameraDirection()
{
	return cameraDirection;
}

glm::vec3 Camera::GetCameraRight()
{
	return cameraRight;
}

glm::vec3 Camera::GetCameraUp()
{
	return cameraUp;
}

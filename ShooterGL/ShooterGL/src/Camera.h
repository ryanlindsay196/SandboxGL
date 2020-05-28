#pragma once
#include "../WorldComponent.h"
#include "Entity.h"

class Camera : public WorldComponent
{
private:
	float drawPosX, drawPosY, drawWidth, drawHeight;//TODO: Use these to implement split screen
	glm::vec3 cameraDirection, cameraRight, cameraUp;
public:
	void Initialize(Entity* newParent);
	virtual void Update(float gameTime) override;

	void Translate(glm::vec3 moveBy);
	void Rotate(glm::vec3 rotationAxis, float rotationAngle);

	glm::vec3 GetCameraDirection();
	glm::vec3 GetCameraRight();
	glm::vec3 GetCameraUp();

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};
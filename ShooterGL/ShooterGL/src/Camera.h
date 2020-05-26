#pragma once
#include "../WorldComponent.h"
#include "Entity.h"

class Camera : public WorldComponent
{
private:
	float drawPosX, drawPosY, drawWidth, drawHeight;//TODO: Use these to implement split screen
public:
	void Initialize(Entity* newParent);
	virtual void Update(float gameTime) override;

	void Translate(glm::vec3 moveBy);
	void Rotate(glm::vec3 rotationAxis, float rotationAngle);

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};
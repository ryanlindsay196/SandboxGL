#pragma once
#include "Component.h"

class WorldComponent : public Component
{
protected:
	glm::mat4 offsetTransform;

	glm::quat rotationQuat;
	glm::mat4 positionOffset, scaleOffset;
	glm::vec3 direction;
	float yaw, pitch, roll;
private:
	void SetEulerAngles(glm::vec3 newEulers);
public:
	virtual void Initialize();
	virtual void Update(float gameTime) override;

	bool isActive;
	glm::mat4 GetOffsetTransform();
	void CalculateTransform();

	void SetTransform(glm::mat4 newTransform);
	void Translate(glm::vec3 translateBy);
	void Rotate(glm::vec3 rotateBy);
	void Scale(glm::vec3 scaleBy);

	void RotateEulerAngles(glm::vec3 newEulers);
	void RotateQuaternion(glm::vec3 rotationAxis, float angle);
	void SetQuaternion(glm::vec3 rotationAxis, float angle);
	glm::vec3 GetEulerAngles();
};
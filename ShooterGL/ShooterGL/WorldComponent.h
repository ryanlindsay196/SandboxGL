#pragma once
#include "Component.h"

class WorldComponent : public Component
{
protected:
	glm::mat4 offsetTransform;

	glm::mat3 positionOffset, rotationOffset, scaleOffset;
public:
	bool isActive;
	inline glm::mat4 GetOffsetTransform() { return offsetTransform; }

	void Translate(glm::vec3 translateBy);
	void Rotate(glm::vec3 rotateBy);
	void Scale(glm::vec3 scaleBy);
};
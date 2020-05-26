#pragma once
#include "Component.h"

class WorldComponent : public Component
{
protected:
	glm::mat4 offsetTransform;
public:
	inline glm::mat4 GetOffsetTransform() { return offsetTransform; }
};
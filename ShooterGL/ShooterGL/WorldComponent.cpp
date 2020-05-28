#include "WorldComponent.h"

void WorldComponent::Translate(glm::vec3 translateBy)
{
	positionOffset += translateBy;
}

void WorldComponent::Rotate(glm::vec3 rotateBy)
{
	rotationOffset += rotateBy;
}

void WorldComponent::Scale(glm::vec3 scaleBy)
{
	scaleOffset += scaleBy;
}

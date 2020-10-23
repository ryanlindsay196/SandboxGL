#include "WorldComponent.h"
#include "glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include "../MathHelperFunctions.h"

void WorldComponent::Initialize()
{
	positionOffset = glm::mat4(1);
	rotationQuat = glm::quat(0,0,0,1);
	scaleOffset = glm::mat4(1);
}

void WorldComponent::Update(float gameTime)
{
	//Set yaw, pitch, and roll based on the rotationQuat
	SetEulerAngles(MathHelperFunctions::Rotation(rotationQuat));
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	offsetTransform = glm::mat4(1);
	CalculateTransform();
	glm::vec3 position = glm::vec3(positionOffset[3][0], positionOffset[3][1], positionOffset[3][2]);
}

glm::mat4 WorldComponent::GetOffsetTransform()
{
	return offsetTransform;
}

void WorldComponent::CalculateTransform()
{
	offsetTransform = positionOffset * glm::toMat4(rotationQuat) * scaleOffset;

	//if (componentParent != nullptr)
	//{
	//	offsetTransform = componentParent->GetTransform() * offsetTransform;
	//}
}

void WorldComponent::SetTransform(glm::mat4 newTransform)
{
	offsetTransform = newTransform;
}

void WorldComponent::Translate(glm::vec3 translateBy)
{
	positionOffset += glm::translate(positionOffset, translateBy);
}

void WorldComponent::Rotate(glm::vec3 rotateBy)
{
	//TODO: make this work
	//rotationOffset += rotateBy;

	rotationQuat = glm::rotate(rotationQuat, rotateBy);
}


void WorldComponent::Scale(glm::vec3 scaleBy)
{
	//TODO: make this work
	scaleOffset = glm::scale(scaleOffset, scaleBy);
}

void WorldComponent::SetEulerAngles(glm::vec3 newEulers)
{
	pitch = newEulers.x;
	yaw = newEulers.y;
	roll = newEulers.z;
}

void WorldComponent::RotateEulerAngles(glm::vec3 newEulers)
{
	pitch += newEulers.x;
	yaw += newEulers.y;
	roll += newEulers.z;
}

void WorldComponent::RotateQuaternion(glm::vec3 rotationAxis, float angle)
{
	rotationAxis = glm::normalize(rotationAxis);
	rotationQuat *= glm::angleAxis(angle, rotationAxis);
}

void WorldComponent::SetQuaternion(glm::vec3 rotationAxis, float angle)
{
	rotationAxis = glm::normalize(rotationAxis);
	rotationQuat = glm::angleAxis(angle, rotationAxis);
}

glm::vec3 WorldComponent::GetEulerAngles()
{
	return glm::vec3(pitch, yaw, roll);
}

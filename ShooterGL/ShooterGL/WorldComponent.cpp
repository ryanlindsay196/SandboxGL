#include "WorldComponent.h"
#include "glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"


void WorldComponent::Update(float gameTime)
{
	SetEulerAngles(Rotation());
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	offsetTransform = glm::mat4(1);
	//if (componentParent != nullptr)
	//	offsetTransform = componentParent->GetTransform();
	//else
	//	offsetTransform = glm::mat4(1);
	//if (componentParent != nullptr)
	//{
	//	//offsetTransform = glm::rotate(offsetTransform, 0.0f, rotationOffset);
	//	//offsetTransform = glm::scale(offsetTransform, scaleOffset);
	//	offsetTransform = glm::translate(offsetTransform, componentParent->GetTranslation());
	//}
	//offsetTransform = glm::translate(offsetTransform, positionOffset);

	//offsetTransform = glm::scale(offsetTransform, scaleOffset);
	//transform = glm::rotate(transform, 40.1f, rotation);
	//offsetTransform = glm::translate(offsetTransform, positionOffset);
	
	//RotateQuaternion(glm::vec3(0, 0, 0.1), 0.02);
	//Translate(glm::vec3(0, 0.0000001, 0));

	offsetTransform = positionOffset * glm::toMat4(rotationQuat) * scaleOffset;

	if (componentParent != nullptr)
	{
		offsetTransform *= componentParent->GetTransform();
	}
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
	//scaleOffset += scaleBy;
}

glm::vec3 WorldComponent::Rotation()
{
	glm::vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (rotationQuat.w * rotationQuat.x + rotationQuat.y * rotationQuat.z);
	double cosr_cosp = +1.0 - 2.0 * (rotationQuat.x * rotationQuat.x + rotationQuat.y * rotationQuat.y);
	angles.x = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (rotationQuat.w * rotationQuat.y - rotationQuat.z * rotationQuat.x);
	if (fabs(sinp) >= 1)
		angles.y = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		angles.y = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (rotationQuat.w * rotationQuat.z + rotationQuat.x * rotationQuat.y);
	double cosy_cosp = +1.0 - 2.0 * (rotationQuat.y * rotationQuat.y + rotationQuat.z * rotationQuat.z);
	angles.z = atan2(siny_cosp, cosy_cosp);

	return angles;
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

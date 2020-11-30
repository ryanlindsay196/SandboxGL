#include "Entity.h"
#include "Component.h"

#include "../MathHelperFunctions.h"
#include "gtx/euler_angles.hpp"

//TODO: find a way to remove this
#include "Renderables/Model.h"
#include "ManagerClasses/LightManager.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#define M_PI   3.14159265358979323846264338327950288

#include "Controller.h"

Entity::~Entity()
{
	components.clear();
}

void Entity::Instantiate(glm::vec3 position, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scale, Entity * newParent)
{
	SetPosition(position);
	rotationQuat = glm::quat(0,0,0,1);
	Rotate(rotationAxis, rotationAngle);
	SetScale(scale);
	if(newParent != nullptr)
		SetParent(newParent);

	glm::vec3 newEulers = glm::eulerAngles(rotationQuat);
	newEulers.x = glm::degrees(newEulers.x);
	newEulers.y = glm::degrees(newEulers.y);
	newEulers.z = glm::degrees(newEulers.z);
	SetEulerAngles(newEulers);
	
	SetNetworkedPosition(position);
	transform = glm::translate(glm::mat4(1), position) * glm::toMat4(rotationQuat) * glm::scale(glm::mat4(1), scale);
}

void Entity::Update(float gameTime)
{
	//Calculate entity direction
	direction = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
	targetDirection = glm::vec3(cos(glm::radians(targetYaw)) * cos(glm::radians(targetPitch)), sin(glm::radians(targetPitch)), sin(glm::radians(targetYaw)) * cos(glm::radians(targetPitch)));

	const float rotationLerpFactor = 0.06f;
	//Apply SLERP (Sperical linear interpolation) to a networked object
	rotationQuat = glm::slerp(rotationQuat,
		MathHelperFunctions::EulerAnglesToQuaternion(glm::vec3(-targetRoll, targetPitch, -targetYaw)),
		rotationLerpFactor);
	pitch += (targetPitch - pitch) * rotationLerpFactor;
	yaw += (targetYaw - yaw) * rotationLerpFactor;
	roll += (targetRoll - roll) * rotationLerpFactor;

	const float positionLerpFactor = 0.06f;
	position = glm::lerp(position, targetPosition, positionLerpFactor);


	//Calculate transform for this frame
	transform = glm::translate(glm::mat4(1), position) * glm::toMat4(rotationQuat) * glm::scale(glm::mat4(1), scale);

	//Update all components
	for (Component* component : components)
	{
		component->Update(gameTime);
	}

}

void Entity::Render(LightManager* lightManager)
{
	for (Component* component : components)
	{
		component->Render(lightManager);
	}
}

Controller * Entity::FindController()
{
	for (Component* component : components)
	{
		Controller* controller = dynamic_cast<Controller*>(component);
		if (controller != nullptr)
		{
			return controller;
		}
	}
	return nullptr;
}

void Entity::SetParent(Entity * newParent)
{
	parent = newParent;
}

void Entity::AddComponent(Component* newComponent)
{
	if (newComponent != nullptr)
	{
		components.push_back(newComponent);
		newComponent->componentParent = this;
	}
}

void Entity::OnCollisionEnter(Entity * entity)
{
	for (Component* component : components)
		component->OnCollisionEnter(entity);
}

void Entity::OnTriggerEnter(Entity * entity)
{
	for (Component* component : components)
		component->OnTriggerEnter(entity);
}

void Entity::Translate(glm::vec3 translateBy)
{
	position += translateBy;
	targetPosition += translateBy;
}

void Entity::Rotate(glm::vec3 rotationAxis, float rotationAngle)
{
	rotationQuat *= glm::angleAxis(rotationAngle, rotationAxis);
	direction = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
	targetDirection = glm::vec3(cos(glm::radians(targetYaw)) * cos(glm::radians(targetPitch)), sin(glm::radians(targetPitch)), sin(glm::radians(targetYaw)) * cos(glm::radians(targetPitch)));
}

void Entity::Scale(glm::vec3 scaleBy)
{
	scale += scaleBy;
}

void Entity::SetPosition(glm::vec3 newPosition)
{
	targetPosition = newPosition - position;
	position = newPosition;
}

void Entity::SetRotation(glm::quat newQuaternion)
{
	rotationQuat = newQuaternion;
	glm::vec3 newEulers = MathHelperFunctions::QuaternionToEulerAngles(rotationQuat);

	targetPitch += newEulers.x - pitch;
	targetYaw += newEulers.y - yaw;
	targetRoll += newEulers.z - roll;

	pitch = newEulers.x;
	yaw = newEulers.y;
	roll = newEulers.z;

	direction = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
	targetDirection = glm::vec3(cos(glm::radians(targetYaw)) * cos(glm::radians(targetPitch)), sin(glm::radians(targetPitch)), sin(glm::radians(targetYaw)) * cos(glm::radians(targetPitch)));
}

void Entity::SetEulerAngles(glm::vec3 newEuler)
{
	targetPitch += newEuler.x - pitch;
	targetYaw += newEuler.y - yaw;
	targetRoll += newEuler.z - roll;

	pitch = newEuler.x;
	yaw = newEuler.y;
	roll = newEuler.z;

	rotationQuat = MathHelperFunctions::EulerAnglesToQuaternion(glm::vec3(-newEuler.z, newEuler.x, -newEuler.y));
}

void Entity::SetScale(glm::vec3 newScale)
{
	scale = newScale;
}

void Entity::SetNetworkedPosition(glm::vec3 newPosition)
{
	targetPosition = newPosition;
}

//Set networked euler angles to rotate this entity towards.
void Entity::SetNetworkedEulerAngles(glm::vec3 newEuler)
{
	targetPitch = newEuler.x;
	targetYaw = newEuler.y;
	targetRoll = newEuler.z;
}

glm::vec3 Entity::GetDirection()
{
	return direction;
}

glm::vec3 Entity::GetTranslation()
{
	return position;
}

glm::quat Entity::GetRotation()
{
	return rotationQuat;
}

glm::vec3 Entity::GetEulerAngles()
{
	return glm::vec3(pitch, yaw, roll);
}

glm::vec3 Entity::GetScale()
{
	return scale;
}

glm::vec3 Entity::GetTargetDirection()
{
	return targetDirection;
}

glm::vec3& Entity::GetDirectionReference()
{
	return direction;
}

glm::vec3 & Entity::GetTranslationReference()
{
	return position;
}

glm::vec3 & Entity::GetScaleReference()
{
	return scale;
}

glm::mat4 Entity::GetTransform()
{
	return transform;
}

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
Entity::~Entity()
{
}

void Entity::Instantiate(glm::vec3 position, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scale, Entity * newParent)
{
	SetPosition(position);
	Rotate(rotationAxis, rotationAngle);
	SetScale(scale);
	if(newParent != nullptr)
		SetParent(newParent);

	glm::vec3 newEulers = glm::eulerAngles(rotationQuat);
	newEulers.x = glm::degrees(newEulers.x);
	newEulers.y = glm::degrees(newEulers.y);
	newEulers.z = glm::degrees(newEulers.z);
	SetEulerAngles(newEulers);
	
	transform = glm::translate(glm::mat4(1), position) * glm::toMat4(rotationQuat) * glm::scale(glm::mat4(1), scale);
}

void Entity::Update(float gameTime)
{
	//Calculate entity direction
	direction = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

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

void Entity::Translate(glm::vec3 translateBy)
{
	position += translateBy;
}

void Entity::Rotate(glm::vec3 rotationAxis, float rotationAngle)
{
	rotationQuat *= glm::angleAxis(rotationAngle, rotationAxis);
	direction = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
}

void Entity::Scale(glm::vec3 scaleBy)
{
	scale += scaleBy;
}

void Entity::SetPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

void Entity::SetEulerAngles(glm::vec3 newEuler)
{
	pitch = newEuler.x;
	yaw = newEuler.y;
	roll = newEuler.z;

	rotationQuat = glm::toQuat(glm::orientate3(glm::vec3(glm::radians(-newEuler.z), glm::radians(newEuler.x), glm::radians(-newEuler.y))));
}

void Entity::SetScale(glm::vec3 newScale)
{
	scale = newScale;
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

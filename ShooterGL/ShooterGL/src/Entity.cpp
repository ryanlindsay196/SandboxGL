#include "Entity.h"
#include "Component.h"

//TODO: find a way to remove this
#include "Renderables/Model.h"
#include "ManagerClasses/LightManager.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
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
	pitch =		(newEulers.x);
	yaw =		(newEulers.y);
	roll =		(newEulers.z);

	transform = glm::translate(glm::mat4(1), position) * glm::toMat4(rotationQuat) * glm::scale(glm::mat4(1), scale);
}

void Entity::Update(float gameTime)
{
	//Calculate entity direction
	direction = glm::vec3(direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)), direction.y = sin(glm::radians(pitch)), direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

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
	rotationQuat += glm::angleAxis(rotationAngle, rotationAxis);
}

void Entity::Scale(glm::vec3 scaleBy)
{
	scale += scaleBy;
}

void Entity::SetPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

//void Entity::SetRotation(glm::vec3 newRotation)
//{
//	rotation = glm::vec3(glm::radians(newRotation.x), glm::radians(newRotation.y), glm::radians(newRotation.z));
//}

void Entity::SetEulerAngles(glm::vec3 newEuler)
{
	pitch = newEuler.x;
	yaw = newEuler.y;
	roll = newEuler.z;

	float sx = sin(glm::radians(newEuler.x / 2));
	float sy = sin(glm::radians(-newEuler.y / 2));
	float sz = sin(glm::radians(newEuler.z / 2));
	float cx = cos(glm::radians(newEuler.x / 2));
	float cy = cos(glm::radians(-newEuler.y / 2));
	float cz = cos(glm::radians(newEuler.z / 2));

	rotationQuat = glm::quat(cx*cy*cz + sx * sy*sz,
		sx*cy*cz - cx * sy*sz,
		cx*sy*cz + sx * cy*sz,
		cx*cy*sz - sx * sy*cz);
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

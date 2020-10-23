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
	SetTranslation(position);
	Rotate(rotationAxis, rotationAngle);
	SetScale(scale);
	transform = glm::mat4(1);
	transform = glm::scale(transform, scale);
	transform = glm::rotate(transform, rotationAngle, rotationAxis);
	transform = glm::translate(transform, position);
	if(newParent != nullptr)
		SetParent(newParent);


	glm::quat quat = glm::quat(0, 0, 0, 1);
	quat = glm::angleAxis(rotationAngle, rotationAxis);
	glm::vec3 newEulers = glm::eulerAngles(quat);
	newEulers = rotationAxis;
	pitch =		(newEulers.x);
	yaw =		(newEulers.y);
	roll =		(newEulers.z);
	//yaw = -90.0f;
}

void Entity::Update(float gameTime)
{
	//Rotate(glm::vec3(0.2f, 0, 0));
	//Translate(glm::vec3(0, 0.002f, 0.002f));
	//Scale(glm::vec3(0.001f));
	
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));


	transform = glm::mat4(1);
	//TODO: Make entity scale affect model components' scale
	//transform = glm::rotate(transform, 40.1f, rotation);
	transform = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));

	//TODO: Look at moving this elsewhere because scale should probably done before rotation
	transform = glm::scale(transform, scale);
	//transform = glm::translate(transform, position);
	transform = glm::translate(glm::mat4(1), position) * glm::toMat4(rotationQuat) * glm::scale(glm::mat4(1), scale);

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
	//glm::translate(position, translateBy);
}

void Entity::Rotate(glm::vec3 rotationAxis, float rotationAngle)
{
	rotationQuat *= glm::angleAxis(rotationAngle, rotationAxis);
	//rotation += glm::vec3(glm::radians(rotateBy.x), glm::radians(rotateBy.y), glm::radians(rotateBy.z));

	//rotation.x = fmod(rotation.x, 300 * 3.14f);
	//rotation.y = fmod(rotation.y, 300 * 3.14f);
	//rotation.z = fmod(rotation.z, 300 * 3.14f);
	//glm::rotate(rotation, rotateBy);
}

void Entity::Scale(glm::vec3 scaleBy)
{
	scale += scaleBy;
	//glm::scale(scale, scaleBy);
}

void Entity::SetTranslation(glm::vec3 newPosition)
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

//glm::vec3 Entity::GetRotation()
//{
//	return rotation;
//}

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

//glm::vec3 & Entity::GetRotationReference()
//{
//	return rotation;
//}

glm::vec3 & Entity::GetScaleReference()
{
	return scale;
}

glm::mat4 Entity::GetTransform()
{
	return transform;
}

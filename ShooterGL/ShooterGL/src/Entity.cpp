#include "Entity.h"
#include "Component.h"

//TODO: find a way to remove this
#include "Renderables/Model.h"

Entity::~Entity()
{
}

void Entity::Instantiate(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Entity * newParent)
{
	SetTranslation(position);
	SetRotation(rotation);
	SetScale(scale);
	transform = glm::mat4(1);
	//transform = glm::rotate(transform, 0.4f, glm::vec3(1, 0, 0));
	transform = glm::scale(transform, scale);
	transform = glm::translate(transform, position);
	if(newParent != nullptr)
		SetParent(newParent);

	yaw = -90.0f;
}

void Entity::Update(float gameTime)
{
	//Rotate(glm::vec3(0.2f, 0, 0));
	//Translate(glm::vec3(0, 0.002f, 0.002f));
	//Scale(glm::vec3(0.001f));
	
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));


	transform = glm::mat4(1);
	//transform = glm::rotate(transform, 40.1f, rotation);
	transform = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));

	//TODO: Look at moving this elsewhere?
	transform = glm::scale(transform, scale);
	//transform = glm::translate(transform, position);


	for (Component* component : components)
	{
		component->Update(gameTime);
	}

}

void Entity::SetParent(Entity * newParent)
{
	parent = newParent;
}

void Entity::AddComponent(Component* newComponent)
{
	//TODO: complete function
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

void Entity::Rotate(glm::vec3 rotateBy)
{
	rotation += rotateBy;
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

void Entity::SetRotation(glm::vec3 newRotation)
{
	rotation = glm::vec3(glm::radians(newRotation.x), glm::radians(newRotation.y), glm::radians(newRotation.z));
}

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

glm::vec3 Entity::GetRotation()
{
	return rotation;
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

glm::vec3 & Entity::GetRotationReference()
{
	return rotation;
}

glm::vec3 & Entity::GetScaleReference()
{
	return scale;
}

glm::mat4 Entity::GetTransform()
{
	return transform;
}

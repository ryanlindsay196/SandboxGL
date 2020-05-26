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
	transform = glm::rotate(transform, 0.4f, glm::vec3(1, 0, 0));
	transform = glm::scale(transform, scale);
	transform = glm::translate(transform, position);
	if(newParent != nullptr)
		SetParent(newParent);
}

void Entity::Update(float gameTime)
{
	//Rotate(glm::vec3(0.2f, 0, 0));
	//Translate(glm::vec3(0, 0.002f, 0.002f));
	Scale(glm::vec3(0.001f));

	transform = glm::mat4(1);
	transform = glm::rotate(transform, 0.f, rotation);
	transform = glm::scale(transform, scale);
	transform = glm::translate(transform, position);
}

void Entity::SetParent(Entity * newParent)
{
	parent = newParent;
}

void Entity::AddComponent(Component* newComponent)
{
	//TODO: complete function
	components.push_back(newComponent);
	newComponent->componentParent = this;
}

void Entity::Translate(glm::vec3 translateBy)
{
	position += translateBy;
	//glm::translate(position, translateBy);
}

void Entity::Rotate(glm::vec3 rotateBy)
{
	rotation += rotateBy;
	rotation.x = fmod(rotation.x, 3.14f);
	rotation.y = fmod(rotation.y, 3.14f);
	rotation.z = fmod(rotation.z, 3.14f);
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
	rotation = newRotation;
}

void Entity::SetScale(glm::vec3 newScale)
{
	scale = newScale;
}

glm::mat4 Entity::GetTransform()
{
	return transform;
}

#include "Entity.h"
#include "Component.h"

//TODO: find a way to remove this
#include "Renderables/Model.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::Instantiate(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Entity * newParent)
{
	SetTranslation(position);
	SetRotation(rotation);
	SetScale(scale);
	if(newParent != nullptr)
		SetParent(newParent);
}

void Entity::Update(float gameTime)
{
}

void Entity::SetParent(Entity * newParent)
{
	parent = newParent;
}

void Entity::AddComponent(Component* newComponent)
{
	//TODO: complete function
	components.push_back(newComponent);
}

void Entity::Translate(glm::vec3 translateBy)
{
	position += translateBy;
	//glm::translate(position, translateBy);
}

void Entity::Rotate(glm::vec3 rotateBy)
{
	rotation += rotateBy;
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

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

void Entity::Instantiate(glm::vec4 position, glm::vec4 rotation, glm::vec4 scale, Entity * newParent)
{
	SetTranslation(position);
	SetRotation(rotation);
	SetScale(scale);
	SetParent(newParent);

	AddComponent(new Model());
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

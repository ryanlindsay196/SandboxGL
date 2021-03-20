#include "Component.h"

void Component::Update(float gameTime)
{
}

void Component::OnCollisionEnter(Entity * entity)
{
}

void Component::OnTriggerEnter(Entity * entity)
{
}

unsigned int Component::GetEntityComponentIndex()
{
	return entityComponentIndex;
}

void Component::SetEntityComponentIndex(unsigned int index)
{
	entityComponentIndex = index;
}

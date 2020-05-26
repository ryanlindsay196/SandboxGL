#include "EntityManager.h"
#include "ObjectManager.h"

void EntityManager::Initialize(ObjectManager * in_objectManager)
{
	//TODO: Loop through all files in the entities folder to load entity properties
	objectManager = in_objectManager;

	//InstantiateEntity(EntityProperties());
	//InstantiateEntity(EntityProperties());
	//InstantiateEntity(EntityProperties());
	//InstantiateEntity(EntityProperties());
}

void EntityManager::InstantiateEntity(EntityProperties entityProperties, glm::vec3 startPos, glm::vec3 startRotationAxis, float rotationAngle, glm::vec3 startScale, Entity* parent)
{
	//TODO: Instantiate entity based on entity properties unordered map
	entities.push_back(new Entity());
	entities[entities.size() - 1]->Instantiate(startPos, startRotationAxis, startScale, parent);
}

void EntityManager::Update(float gameTime)
{
	for (Entity* entity : entities)
	{
		entity->Update(gameTime);
	}
}

Entity* EntityManager::GetEntity(int i)
{
	if ((unsigned int)i < entities.size())
		return entities[i];
	return nullptr;
}

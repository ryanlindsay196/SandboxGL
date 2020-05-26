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

void EntityManager::InstantiateEntity(EntityProperties entityProperties)
{
	entities.push_back(new Entity());
	entities[entities.size() - 1]->Instantiate(glm::vec3(), glm::vec3(), glm::vec3(), nullptr);
	entities[entities.size() - 1]->AddComponent(objectManager->modelManager->GetModel(0));
	entities.push_back(new Entity());
	entities[entities.size() - 1]->Instantiate(glm::vec3(), glm::vec3(), glm::vec3(), nullptr);
}

void EntityManager::Update(float gameTime)
{
	for (Entity* entity : entities)
	{
		entity->Update(gameTime);
	}
}

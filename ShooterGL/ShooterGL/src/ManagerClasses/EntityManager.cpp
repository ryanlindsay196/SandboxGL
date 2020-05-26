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
	entities.push_back(new Entity());
	entities[entities.size() - 1]->Instantiate(startPos, startRotationAxis, startScale, parent);
	entities[entities.size() - 1]->AddComponent(objectManager->modelManager->GetModel(0));
	entities.push_back(new Entity());
	entities[entities.size() - 1]->Instantiate(glm::vec3(0, 0, 0), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), parent);
}

void EntityManager::Update(float gameTime)
{
	for (Entity* entity : entities)
	{
		entity->Update(gameTime);
	}
}

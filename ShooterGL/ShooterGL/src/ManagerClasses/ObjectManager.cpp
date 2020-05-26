#include "ObjectManager.h"
#include "EntityManager.h"

void ObjectManager::Initialize()
{
	entityManager = new EntityManager();
	modelManager = new ModelManager();
	textureManager = new TextureManager();
	entityManager->Initialize(this);
	modelManager->Initialize(this);

	modelManager->LoadModel(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 0.5f, glm::vec3(1, 1, 1));
	modelManager->LoadModel(glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), 0.2f, glm::vec3(0.4f, 0.9f, 0.4f));
	entityManager->InstantiateEntity(EntityManager::EntityProperties(), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.4f, glm::vec3(1, 1, 1), nullptr);
	//move model transformations by entity transformations
}

void ObjectManager::Update(float gameTime)
{
	modelManager->UpdateModels(gameTime);
	entityManager->Update(gameTime);
}

void ObjectManager::Render()
{
	modelManager->RenderModels();
}

#include "ObjectManager.h"
#include "EntityManager.h"

void ObjectManager::Initialize()
{
	entityManager = new EntityManager();
	modelManager = new ModelManager();
	textureManager = new TextureManager();
	entityManager->Initialize(this);
	modelManager->Initialize(this);

	modelManager->LoadModel();
	modelManager->LoadModel();

	entityManager->InstantiateEntity(EntityManager::EntityProperties());
	move model transformations by entity transformations
}

void ObjectManager::Update(float gameTime)
{
	entityManager->Update(gameTime);
}

void ObjectManager::Render()
{
	modelManager->RenderModels();
}

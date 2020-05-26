#include "ObjectManager.h"

void ObjectManager::Initialize()
{
	entityManager = new EntityManager();
	modelManager = new ModelManager();
	textureManager = new TextureManager();
}

void ObjectManager::Update(float gameTime)
{
	entityManager->Update(gameTime);
}

void ObjectManager::Render()
{
	modelManager->RenderModels();
}

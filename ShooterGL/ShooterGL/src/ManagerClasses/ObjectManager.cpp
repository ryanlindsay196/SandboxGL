#include "ObjectManager.h"
#include "EntityManager.h"

void ObjectManager::Initialize()
{
	entityManager = new EntityManager();
	modelManager = new ModelManager();
	textureManager = new TextureManager();
	cameraManager = new CameraManager();
	entityManager->Initialize(this);
	modelManager->Initialize(this);
	cameraManager->Initialize(this);

	modelManager->LoadModel(glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1, 1, 1));
	//modelManager->LoadModel(glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), 0.0f, glm::vec3(1.f, 1.f, 1.f));
	entityManager->InstantiateEntity(EntityManager::EntityProperties(), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1, 1, 1), nullptr);
	entityManager->InstantiateEntity(EntityManager::EntityProperties(), glm::vec3(0, 0.0f, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1, 1, 1), nullptr);

	entityManager->GetEntity(1)->AddComponent(modelManager->GetModel(1));
	entityManager->GetEntity(0)->AddComponent(modelManager->GetModel(0));

	cameraManager->CreateCamera(nullptr);
	entityManager->GetEntity(0)->AddComponent(cameraManager->GetCamera(0));

	//move model transformations by entity transformations
}

void ObjectManager::Update(float gameTime)
{

	entityManager->GetEntity(1)->Translate(glm::vec3(0.001f, 0.f, 0.f));
	entityManager->GetEntity(0)->Translate(glm::vec3(-0.0001f, 0.f, 0.f));

	modelManager->UpdateModels(gameTime);
	entityManager->Update(gameTime);
	cameraManager->Update();
}

void ObjectManager::Render()
{
	modelManager->RenderModels();
}

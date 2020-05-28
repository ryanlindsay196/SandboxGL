#include "ObjectManager.h"
#include "EntityManager.h"
#include "GLFW/glfw3.h"

void ObjectManager::Initialize(GLFWwindow* window)
{
	entityManager = new EntityManager();
	modelManager = new ModelManager();
	textureManager = new TextureManager();
	cameraManager = new CameraManager();
	controllerManager = new ControllerManager();
	entityManager->Initialize(this);
	modelManager->Initialize(this);
	cameraManager->Initialize(this);
	controllerManager->Initialize(window);

	modelManager->LoadModel(glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), 0.0f, glm::vec3(0.f, 0.f, 0.f));
	//modelManager->LoadModel(glm::vec3(0, 1, -2), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1, 1, 1));
	modelManager->LoadModel(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), 0.0f, glm::vec3(0.f, 0.f, 0.f));
	entityManager->InstantiateEntity(EntityManager::EntityProperties(), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1, 1, 1), nullptr);
	entityManager->InstantiateEntity(EntityManager::EntityProperties(), glm::vec3(0, 0.0f, -1), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1, 1, 1), nullptr);

	entityManager->GetEntity(1)->AddComponent(modelManager->GetModel(1));
	entityManager->GetEntity(0)->AddComponent(modelManager->GetModel(0));

	cameraManager->CreateCamera(entityManager->GetEntity(0));
	//entityManager->GetEntity(0)->AddComponent(cameraManager->GetCamera(0));

	entityManager->GetEntity(0)->AddComponent(controllerManager->CreateController(entityManager->GetEntity(0)));
	//move model transformations by entity transformations

	//Makeshift floor
	modelManager->LoadModel(glm::vec3(-1, -1, -1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(-1, -1, 0), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, -1, -1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, -1, 0), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, -1, 1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(1, -1, -1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(1, -1, 0), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(-1, -1, 2), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, -1, 2), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(1, -1, 2), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(-1, -1, 3), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, -1, 3), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(1, -1, 3), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(-1, -1, 4), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, -1, 4), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(1, -1, 4), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(-1, -1, 5), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, -1, 5), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(1, -1, 5), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(-1, -1, 6), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(0, -1, 6), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	modelManager->LoadModel(glm::vec3(1, -1, 6), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));

}

void ObjectManager::Update(float gameTime)
{
	//cameraManager->GetCamera(0)->Rotate(glm::vec3(1, 0, 0), 0.02f);
	//cameraManager->GetCamera(0)->Translate(glm::vec3(0, 0, -0.011f));
	//entityManager->GetEntity(0)->Rotate(glm::vec3(1, 0, 0));
	//modelManager->GetModel(0)->r
	//modelManager->GetModel
	//entityManager->GetEntity(1)->Translate(glm::vec3(0.f, 0.f, -0.001f));
	//entityManager->GetEntity(0)->Translate(glm::vec3(0.f, 0.001f, 0.f));

	modelManager->UpdateModels(gameTime);
	entityManager->Update(gameTime);
	cameraManager->Update();
}

void ObjectManager::Render()
{
	modelManager->RenderModels();
}

#include "ObjectManager.h"
#include "EntityManager.h"
#include "GLFW/glfw3.h"
#include "Renderables/Mesh.h"

#include "EntityManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "ControllerManager.h"
#include "LightManager.h"
#include "ShaderManager.h"
#include "ManagerClasses/NetworkManager.h"
#include "PhysicsManager.h"
#include "NetworkManager.h"
#include "PlayerManager.h"

#include <iostream>

void ObjectManager::Initialize(GLFWwindow* window)
{
	//TODO: Load fixedUpdateTime and other variables (To Be Determined) from a project settings file
	//30 fps = 2 / 60
	//60 fps = 1 / 60
	fixedUpdateMaxTime = 2.f / 60.f;

	entityManager = new EntityManager();
	modelManager = new ModelManager();
	textureManager = new TextureManager();
	cameraManager = new CameraManager();
	controllerManager = new ControllerManager();
	lightManager = new LightManager();
	shaderManager = new ShaderManager();
	physicsManager = new PhysicsManager();
	playerManager = new PlayerManager();
	
	LoadScene("Resources/Scenes/Test2.scene", window);

	//TODO: Make sure networkManager only loads and updates when there is a networked object in the scene
	networkManager = new NetworkManager();
	networkManager->Initialize(entityManager, controllerManager);

}

void ObjectManager::Update(float gameTime)
{

#pragma region TODO: Temporary. Please Move elsewhere or refactor
	//while (true)
	{
		networkManager->Update(0.01f);
	}
	//networkManager->DeInitialize();
#pragma endregion


	fixedUpdateTimer += gameTime;
	entityManager->Update(gameTime);
	if (fixedUpdateTimer >= fixedUpdateMaxTime)
	{
		physicsManager->FixedUpdate(fixedUpdateTimer);
		fixedUpdateTimer = 0;
	}
	cameraManager->Update();
}

void ObjectManager::Render()
{
	entityManager->Render(lightManager);
}

void ObjectManager::LoadScene(std::string scenePath, GLFWwindow* window)
{
	//TODO: Fix memory leak somewhere in this function

	lightManager->Initialize();

	controllerManager->Initialize(window);
	entityManager->Initialize(this);
	modelManager->Initialize(this);
	cameraManager->Initialize(this);

	physicsManager->Initialize(glm::vec3(24, 24, 24), glm::vec3(1, 1, 1));

	entityManager->LoadScene(scenePath);

	modelManager->LoadShaders();

	modelManager->UnloadModelData();
	shaderManager->UnloadShaders();
	textureManager->UnloadTextures();
	std::cout << "Finished loading scene: " << scenePath << std::endl;
}


#ifdef DEBUG
#include "Debug/DrawRay.h"
#endif //DEBUG

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


ObjectManager* ObjectManager::instance = 0;

ObjectManager::ObjectManager()
{
}

ObjectManager * ObjectManager::GetInstance()
{
	if (!instance)
		instance = new ObjectManager();
	return instance;
}

void ObjectManager::Initialize(GLFWwindow* window)
{
	//TODO: when calling fixedUpdate, use fixedUpdateMaxTime instead of time elapsed since last fixed update

	//TODO: Load fixedUpdateTime and other variables (To Be Determined) from a project settings file
	//30 fps = 2 / 60
	//60 fps = 1 / 60
	fixedUpdateMaxTime = 2.f / 60.f;

	entityManager = EntityManager::GetInstance();
	modelManager = ModelManager::GetInstance();
	textureManager = TextureManager::GetInstance();
	cameraManager = CameraManager::GetInstance();
	controllerManager = ControllerManager::GetInstance();
	lightManager = LightManager::GetInstance();
	shaderManager = ShaderManager::GetInstance();
	physicsManager = PhysicsManager::GetInstance();
	playerManager = PlayerManager::GetInstance();
	
	LoadScene("Resources/Scenes/Test2.scene", window);

	//TODO: Make sure networkManager only loads and updates when there is a networked object in the scene
	networkManager = NetworkManager::GetInstance();
	//networkManager->Initialize(entityManager, controllerManager);

}

void ObjectManager::Update(float gameTime)
{
#ifdef DEBUG
	DebugLines::ClearVertices();
#endif //DEBUG

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



#ifdef DEBUG
	const int totalGridLines = 30;
	for (int i = 0; i <= totalGridLines; i++)
	{
		DebugLines::AddLine(glm::vec3(-totalGridLines / 2, 5, i - (totalGridLines / 2)), glm::vec3(totalGridLines / 2, 5, i - (totalGridLines / 2)), nullptr);
		DebugLines::AddLine(glm::vec3(i - totalGridLines / 2, 5, -totalGridLines / 2), glm::vec3(i - totalGridLines / 2, 5, totalGridLines / 2), nullptr);
	}

	glDepthFunc(GL_ALWAYS);

	DebugLines::GetInstance()->DrawLines();
	glDepthFunc(GL_LESS);
#endif //DEBUG
}

void ObjectManager::LoadScene(std::string scenePath, GLFWwindow* window)
{
	//TODO: Fix memory leak somewhere in this function

#ifdef DEBUG
	DebugLines::ClearVertices();
#endif //DEBUG

	lightManager->Initialize();

	controllerManager->Initialize(window);
	entityManager->Initialize();
	modelManager->Initialize();
	cameraManager->Initialize();

	physicsManager->Initialize(glm::vec3(24, 24, 24), glm::vec3(1, 1, 1));

	entityManager->LoadScene(scenePath);

	modelManager->LoadShaders();

	modelManager->UnloadModelData();
	shaderManager->UnloadShaders();
	textureManager->UnloadTextures();
	std::cout << "Finished loading scene: " << scenePath << std::endl;
}

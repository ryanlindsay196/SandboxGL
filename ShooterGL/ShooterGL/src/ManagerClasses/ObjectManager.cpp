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
#include "PhysicsManager.h"

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
	
	LoadScene("Resources/Scenes/Test2.scene", window);
}

void ObjectManager::Update(float gameTime)
{
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

#pragma once

#include "EntityManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "ControllerManager.h"
//TODO: include all managers
struct GLFWwindow;

class ObjectManager
{
public:
	EntityManager* entityManager;
	ModelManager* modelManager;
	TextureManager* textureManager;
	CameraManager* cameraManager;
	ControllerManager* controllerManager;

	void Initialize(GLFWwindow* window);
	void Update(float gameTime);
	void Render();
};
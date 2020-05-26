#pragma once

#include "EntityManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
//TODO: include all managers

class ObjectManager
{
public:
	EntityManager* entityManager;
	ModelManager* modelManager;
	TextureManager* textureManager;
	CameraManager* cameraManager;

	void Initialize();
	void Update(float gameTime);
	void Render();
};
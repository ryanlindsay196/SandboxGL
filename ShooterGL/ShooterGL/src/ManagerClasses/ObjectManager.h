#pragma once

class EntityManager;
class ModelManager;
class TextureManager;
class CameraManager;
class ControllerManager;
class LightManager;
class ShaderManager;

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
	LightManager* lightManager;
	ShaderManager* shaderManager;

	void Initialize(GLFWwindow* window);
	void Update(float gameTime);
	void Render();
};
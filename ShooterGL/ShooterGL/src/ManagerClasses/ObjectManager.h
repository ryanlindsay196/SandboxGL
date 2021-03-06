#pragma once
#include <string>

class EntityManager;
class ModelManager;
class TextureManager;
class CameraManager;
class ControllerManager;
class LightManager;
class ShaderManager;
class PhysicsManager;
class NetworkManager;
class PlayerManager;

//TODO: include all managers
struct GLFWwindow;

class ObjectManager
{
private:
	ObjectManager();
	static ObjectManager* instance;
public:
	static ObjectManager* GetInstance();

	EntityManager* entityManager;
	ModelManager* modelManager;
	TextureManager* textureManager;
	CameraManager* cameraManager;
	ControllerManager* controllerManager;
	LightManager* lightManager;
	ShaderManager* shaderManager;
	PhysicsManager* physicsManager;
	NetworkManager* networkManager;
	PlayerManager* playerManager;

	void Initialize(GLFWwindow* window);
	void Update(float gameTime);
	void Render();
	void LoadScene(std::string scenePath, GLFWwindow* window);
private:
	float fixedUpdateTimer, fixedUpdateMaxTime;
};
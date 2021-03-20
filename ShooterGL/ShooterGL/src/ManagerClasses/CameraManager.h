#pragma once
#include "../WorldComponent.h"
#include "Camera.h"
#include "BaseManager.h"
//#include "ObjectManager.h"
class ObjectManager;

class CameraManager : public BaseManager
{
private:
	std::vector<Camera> cameras;
	static CameraManager* instance;
	CameraManager();
public:
	static CameraManager* GetInstance();
	void Initialize();
	Camera* CreateCamera(Entity* newParent);
	Camera* GetCamera(int i);
	void Update();
};
#pragma once
#include "../WorldComponent.h"
#include "Camera.h"
//#include "ObjectManager.h"
class ObjectManager;

class CameraManager
{
private:
	ObjectManager* m_objectManager;
	std::vector<Camera> cameras;
public:
	void Initialize(ObjectManager* objectManager);
	Camera& CreateCamera(Entity* newParent);
	Camera* GetCamera(int i);
	void Update();
};
#include "CameraManager.h"
//#include "ObjectManager.h"

CameraManager* CameraManager::instance = 0;

CameraManager::CameraManager()
{
}

CameraManager * CameraManager::GetInstance()
{
	if (!instance)
		instance = new CameraManager();
	return instance;
}

void CameraManager::Initialize()
{
	cameras.clear();
	cameras.reserve(4);
}

Camera* CameraManager::CreateCamera(Entity * newParent, const unsigned int newEntityComponentIndex)
{
	AddComponent<Camera>(cameras, newEntityComponentIndex, newParent);
	cameras[cameras.size() - 1].Initialize();
	return &cameras[cameras.size() - 1];
}

Camera * CameraManager::GetCamera(int i)
{
	if ((unsigned int)i < cameras.size())
		return &cameras[i];
	return nullptr;
}

void CameraManager::Update()
{
}

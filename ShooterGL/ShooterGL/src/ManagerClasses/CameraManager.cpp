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

Camera* CameraManager::CreateCamera(Entity * newParent)
{
	cameras.push_back(Camera());
	cameras[cameras.size() - 1].Initialize(newParent);
	//cameras[cameras.size() - 1].componentParent = newParent;
	//newParent->AddComponent(&cameras[cameras.size() - 1]);
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

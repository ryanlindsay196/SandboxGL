#include "CameraManager.h"

void CameraManager::Initialize(ObjectManager * objectManager)
{
	m_objectManager = objectManager;
}

Camera* CameraManager::CreateCamera(Entity * newParent)
{
	cameras.push_back(Camera());
	cameras[cameras.size() - 1].Initialize(nullptr);
	cameras[cameras.size() - 1].componentParent = newParent;
	newParent->AddComponent(&cameras[cameras.size() - 1]);
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

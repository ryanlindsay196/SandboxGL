#include "ControllerManager.h"

ControllerManager* ControllerManager::instance = 0;

ControllerManager::ControllerManager()
{
}

ControllerManager * ControllerManager::GetInstance()
{
	if (!instance)
		instance = new ControllerManager();
	return instance;
}

ControllerManager::~ControllerManager()
{
}

void ControllerManager::Initialize(GLFWwindow * window)
{
	controllers.clear();
	m_window = window;
	controllers.reserve(4);
}

Controller * ControllerManager::CreateController(Entity* newCameraParentEntity, unsigned int newEntityComponentIndex)
{
	AddComponent<Controller>(controllers, newEntityComponentIndex, newCameraParentEntity);
	controllers[controllers.size() - 1].Initialize(m_window);
	//controllers[controllers.size() - 1].componentParent = newCameraParentEntity;
	return &controllers[controllers.size() - 1];
}

Controller * ControllerManager::GetController(int i)
{
	if ((unsigned int)i < controllers.size())
		return &controllers[i];
	return nullptr;
}

unsigned int ControllerManager::TotalControllers()
{
	return controllers.size();
}

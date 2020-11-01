#include "ControllerManager.h"

void ControllerManager::Initialize(GLFWwindow * window)
{
	controllers.clear();
	m_window = window;
}

Controller * ControllerManager::CreateController(Entity* newCameraParentEntity)
{
	controllers.push_back(Controller());
	controllers[controllers.size() - 1].Initialize(m_window);
	controllers[controllers.size() - 1].componentParent = newCameraParentEntity;
	return &controllers[controllers.size() - 1];
}

#include "LightManager.h"

void LightManager::Initialize()
{
}

Light * LightManager::AddLight(glm::vec3 position, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scale)
{
	m_lights.push_back(new Light());
	m_lights[m_lights.size() - 1]->Initialize(glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), position, rotationAxis, rotationAngle, scale);
	return m_lights[m_lights.size() - 1];
}

Light * LightManager::GetLight(unsigned int i)
{
	if(i < m_lights.size())
		return m_lights[i];
	return nullptr;
}

unsigned int LightManager::TotalLights()
{
	return m_lights.size();
}

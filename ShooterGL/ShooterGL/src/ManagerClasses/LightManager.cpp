#include "LightManager.h"

void LightManager::Initialize()
{
}

Light* LightManager::AddLight(glm::vec3 position, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scale, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse)
{
	m_lights.push_back(new Light());
	m_lights[m_lights.size() - 1]->Initialize(ambient, specular, diffuse, position, rotationAxis, rotationAngle, scale);
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

#include "LightManager.h"

void LightManager::Initialize()
{
}

Light* LightManager::AddLight(std::vector<std::string>& lightProperties)
{
	m_lights.push_back(new Light());
	m_lights[m_lights.size() - 1]->Initialize(lightProperties);
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

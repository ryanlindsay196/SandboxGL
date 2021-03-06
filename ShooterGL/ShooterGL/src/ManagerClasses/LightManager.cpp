#include "LightManager.h"

LightManager* LightManager::instance = 0;

LightManager::LightManager()
{
}

LightManager * LightManager::GetInstance()
{
	if (!instance)
		instance = new LightManager();
	return instance;
}

void LightManager::Initialize()
{
	for (auto light : m_lights)
		delete(light);
	m_lights.clear();
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

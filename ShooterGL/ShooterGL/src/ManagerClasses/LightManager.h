#pragma once
#include "Renderables/Light.h"
#include "BaseManager.h"

class LightManager : public BaseManager
{
private:
	static LightManager* instance;
	LightManager();
	LightManager(LightManager&) = delete;
public:
	static LightManager* GetInstance();

	void Initialize();

	Light* AddLight(std::vector<std::string>& lightProperties, unsigned int newEntityComponentIndex, Entity* parentEntity);

	Light* GetLight(unsigned int i);
	unsigned int TotalLights();

private:
	std::vector<Light> m_lights;
};
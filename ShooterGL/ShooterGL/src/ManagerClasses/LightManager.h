#pragma once
#include "Renderables/Light.h"
class LightManager
{
private:
	static LightManager* instance;
	LightManager();
public:
	static LightManager* GetInstance();

	void Initialize();

	Light* AddLight(std::vector<std::string>& lightProperties);

	Light* GetLight(unsigned int i);
	unsigned int TotalLights();

private:
	std::vector<Light*> m_lights;
};
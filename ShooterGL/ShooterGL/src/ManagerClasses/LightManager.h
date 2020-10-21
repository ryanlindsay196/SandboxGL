#pragma once
#include "Renderables/Light.h"
class LightManager
{
public:
	void Initialize();

	Light* AddLight(std::vector<std::string>& lightProperties);

	Light* GetLight(unsigned int i);
	unsigned int TotalLights();

private:
	std::vector<Light*> m_lights;
};
#pragma once
#include "Renderables/Light.h"
class LightManager
{
public:
	void Initialize();

	Light* AddLight(glm::vec3 position, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scale);

	Light* GetLight(unsigned int i);
	unsigned int TotalLights();

private:
	std::vector<Light*> m_lights;
};
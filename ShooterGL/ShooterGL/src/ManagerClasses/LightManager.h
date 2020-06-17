#pragma once
#include "Renderables/Light.h"
class LightManager
{
public:
	void Initialize();

	Light* AddLight(glm::vec3 position, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scale, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse);

	Light* GetLight(unsigned int i);
	unsigned int TotalLights();

private:
	std::vector<Light*> m_lights;
};
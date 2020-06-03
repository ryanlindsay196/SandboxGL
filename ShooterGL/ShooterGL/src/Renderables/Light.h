#pragma once

#include "../WorldComponent.h"

class Light : public WorldComponent
{
	//glm::vec3 color;
	//float intensity;
private:
	glm::vec3 ambient, specular, diffuse;

public:
	void Initialize(glm::vec3 newAmbient, glm::vec3 newSpecular, glm::vec3 newDiffuse);
	Light();
	~Light();

	glm::vec3 GetAmbient();
	glm::vec3 GetSpecular();
	glm::vec3 GetDiffuse();

	void SetAmbient(glm::vec3 newAmbient);
	void SetSpecular(glm::vec3 newSpecular);
	void SetDiffuse(glm::vec3 newDiffuse);
};
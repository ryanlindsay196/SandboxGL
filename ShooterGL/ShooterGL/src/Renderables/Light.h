#pragma once

#include "../WorldComponent.h"

class Light : public WorldComponent
{
	//glm::vec3 color;
	//float intensity;
private:
	glm::vec3 ambient, specular, diffuse;
	float constant = 0;
	float linear = 0;
	float quadratic = 0;
public:
	void Initialize(std::vector<std::string>& lightProperties);
	Light();
	~Light();
	void ReadPointLightData(std::vector<std::string>& dataString);

	glm::vec3 GetAmbient();
	glm::vec3 GetSpecular();
	glm::vec3 GetDiffuse();

	glm::vec3& GetAmbientReference();
	glm::vec3& GetSpecularReference();
	glm::vec3& GetDiffuseReference();

	float GetConstant();
	float& GetConstantReference();
	float GetLinear();
	float& GetLinearReference();
	float GetQuadratic();
	float& GetQuadraticReference();

	void SetAmbient(glm::vec3 newAmbient);
	void SetSpecular(glm::vec3 newSpecular);
	void SetDiffuse(glm::vec3 newDiffuse);
};
#include "Light.h"
//#include "gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include "FileReader.h"
#include <algorithm>
#include <iostream>

void Light::Initialize(std::vector<std::string>& lightProperties)
{
	//ambient = newAmbient;
	//specular = newSpecular;
	//diffuse = newDiffuse;
	offsetTransform = glm::mat4(1);
	positionOffset = glm::mat4(1);
	scaleOffset = glm::mat4(1);
	rotationQuat = glm::quat(0, 0, 0, 1);
	ReadPointLightData(lightProperties);
	offsetTransform = positionOffset * glm::toMat4(rotationQuat) * scaleOffset;
}

Light::Light()
{
}

Light::~Light()
{
}

void Light::ReadPointLightData(std::vector<std::string>& dataString)
{
	glm::vec3 rotationAxis;
	float rotationAngle;
	for (std::string data : dataString)
	{
		if (data == "")
			continue;
		data.erase(std::remove(data.begin(), data.end(), '\t'), data.end());
		data.erase(std::remove(data.begin(), data.end(), ' '), data.end());
		std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(data, ":");
		if (keyValuePair.first == "Ambient")
		{
			ambient = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Specular")
		{
			specular = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Diffuse")
		{
			diffuse = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Position")
		{
			glm::vec3 position = ParseVector(keyValuePair.second);
			positionOffset = glm::translate(positionOffset, position);
		}
		else if (keyValuePair.first == "EulerAngles")
		{
			rotationAxis = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "RotationAngle")
		{
			rotationAngle = strtof(keyValuePair.second.c_str(), nullptr);
		}
		else if (keyValuePair.first == "Scale")
		{
			glm::vec3 scale = ParseVector(keyValuePair.second);
			scaleOffset = glm::scale(scaleOffset, scale);
		}
		else if (keyValuePair.first == "Constant")
		{
			constant = strtof(keyValuePair.second.c_str(), nullptr);
		}
		else if (keyValuePair.first == "Linear")
		{
			linear = strtof(keyValuePair.second.c_str(), nullptr);
		}
		else if (keyValuePair.first == "Quadratic")
		{
			quadratic = strtof(keyValuePair.second.c_str(), nullptr);

		}
		else
		{
			std::cout << "The " << keyValuePair.first << " is not supported in PointLight" << std::endl;
		}
	}

	rotationQuat *= glm::angleAxis(rotationAngle, rotationAxis);
	rotationQuat = glm::rotate(rotationQuat, glm::vec3(M_PI / 2, 0, 0));
}

glm::vec3 Light::GetAmbient()
{
	return ambient;
}

glm::vec3 Light::GetSpecular()
{
	return specular;
}

glm::vec3 Light::GetDiffuse()
{
	return diffuse;
}

glm::vec3& Light::GetAmbientReference()
{
	return ambient;
}

glm::vec3& Light::GetSpecularReference()
{
	return specular;
}

glm::vec3& Light::GetDiffuseReference()
{
	return diffuse;
}

float Light::GetConstant()
{
	return constant;
}

float & Light::GetConstantReference()
{
	return constant;
}

float Light::GetLinear()
{
	return linear;
}

float & Light::GetLinearReference()
{
	return linear;
}

float Light::GetQuadratic()
{
	return quadratic;
}

float & Light::GetQuadraticReference()
{
	return quadratic;
}

void Light::SetAmbient(glm::vec3 newAmbient)
{
	ambient = newAmbient;
}

void Light::SetSpecular(glm::vec3 newSpecular)
{
	specular = newSpecular;
}

void Light::SetDiffuse(glm::vec3 newDiffuse)
{
	diffuse = newDiffuse;
}

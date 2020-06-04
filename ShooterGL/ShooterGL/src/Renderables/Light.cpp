#include "Light.h"
//#include "gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"

void Light::Initialize(glm::vec3 newAmbient, glm::vec3 newSpecular, glm::vec3 newDiffuse, glm::vec3 position, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scale)
{
	ambient = newAmbient;
	specular = newSpecular;
	diffuse = newDiffuse;

	offsetTransform = glm::mat4(1);
	positionOffset = glm::mat4(1);
	scaleOffset = glm::mat4(1);
	scaleOffset = glm::scale(scaleOffset, scale);
	rotationQuat = glm::quat(0, 0, 0, 1);
	rotationQuat *= glm::angleAxis(rotationAngle, rotationAxis);
	rotationQuat = glm::rotate(rotationQuat, glm::vec3(M_PI / 2, 0, 0));
	positionOffset = glm::translate(positionOffset, position);
	offsetTransform = positionOffset * glm::toMat4(rotationQuat);
}

Light::Light()
{
}

Light::~Light()
{
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

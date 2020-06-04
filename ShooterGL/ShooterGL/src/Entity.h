#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>

class Component;

class Entity
{
public:
	//Entity();
	~Entity();

	void Instantiate(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Entity* newParent);
	void Update(float gameTime);

	void SetParent(Entity* newParent);
	void AddComponent(Component* newComponent);

	void Translate(glm::vec3 translateBy);
	void Rotate(glm::vec3 rotateBy);
	void Scale(glm::vec3 scaleBy);

	void SetTranslation(glm::vec3 newPosition);
	void SetRotation(glm::vec3 newRotation);
	void SetEulerAngles(glm::vec3 newEuler);
	void SetScale(glm::vec3 newScale);

	glm::vec3 GetDirection();
	glm::vec3 GetTranslation();
	glm::vec3 GetRotation();
	glm::vec3 GetEulerAngles();
	glm::vec3 GetScale();

	glm::vec3& GetDirectionReference();
	glm::vec3& GetTranslationReference();
	glm::vec3& GetRotationReference();
	//glm::vec3& GetEulerAnglesReference();
	glm::vec3& GetScaleReference();

	glm::mat4 GetTransform();

private:
	Entity* parent;

	glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 direction;
	float roll, pitch, yaw;

	std::vector<Component*> components;
	std::vector<std::string> tags;
};
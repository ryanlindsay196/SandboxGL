#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>

class Component;

class Entity
{
public:
	Entity();
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
	void SetScale(glm::vec3 newScale);

private:
	Entity* parent;

	glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	std::vector<Component*> components;
	std::vector<std::string> tags;
};
#pragma once
#include "Component.h"
struct GLFWwindow;

class Controller : public Component
{
	//Needs hot swappable controls
	//Hot swapping for controls 
public:
	void Initialize(GLFWwindow* newWindow);
	void AddKeyBinding(/*Function to call*/);
	virtual void Update(float gameTime) override;
	//TODO: Add keyboard/controller support
	//TODO: Add support for different entity states. These should be hot swappable
	void Move(glm::vec3 direction, float moveSpeed);
	//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
private:
	GLFWwindow* window;
};
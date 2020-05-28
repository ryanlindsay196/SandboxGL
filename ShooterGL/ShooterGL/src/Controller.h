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
	//Needs to have keyboard/controller support
	//Needs to reference the controlled object (player) using componentParent
	//Needs to support different entity states. These should be hot swappable
private:
	GLFWwindow* window;
};
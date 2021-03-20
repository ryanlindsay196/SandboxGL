#pragma once
#include "Controller.h"
#include <vector>
#include "BaseManager.h"

struct GLFWwindow;

class ControllerManager : public BaseManager
{
private:
	std::vector<Controller> controllers;

	ControllerManager();
	static ControllerManager* instance;
public:
	static ControllerManager* GetInstance();
	~ControllerManager();

	void Initialize(GLFWwindow* window);
	Controller* CreateController(Entity* newCameraParentEntity, unsigned int newEntityComponentIndex);
	Controller* GetController(int i);
	unsigned int TotalControllers();

	GLFWwindow* m_window;

	//Variables and functions for reading and translating keycodes 

	//Variables and functions for translating keys into triggers for actions
};
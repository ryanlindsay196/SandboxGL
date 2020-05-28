#include "Controller.h"
#include "GLFW/glfw3.h"

void Controller::Initialize(GLFWwindow * newWindow)
{
	window = newWindow;
}

void Controller::AddKeyBinding()
{
}

void Controller::Update(float gameTime)
{
	if (glfwGetKey(window, GLFW_KEY_W))
		componentParent->Translate(glm::vec3(0, 0, -0.01f) * gameTime);
	if (glfwGetKey(window, GLFW_KEY_S))
		componentParent->Translate(glm::vec3(0, 0, 0.01f) * gameTime);
}

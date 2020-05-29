#include "Controller.h"
#include "GLFW/glfw3.h"

//TODO: Remove sstream
#include <sstream>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
float lastX, lastY, xoffset, yoffset;
bool firstMouse;

void Controller::Initialize(GLFWwindow* newWindow)
{
	window = newWindow;
	glfwSetCursorPosCallback(window, mouse_callback);
}

void Controller::AddKeyBinding()
{
}

void Controller::Update(float gameTime)
{
	glm::vec3 moveDirection = glm::vec3(0);
	if (glfwGetKey(window, GLFW_KEY_W))
		moveDirection += componentParent->GetDirection();
	if (glfwGetKey(window, GLFW_KEY_S))
		moveDirection += -componentParent->GetDirection();
	if (glfwGetKey(window, GLFW_KEY_A))
		moveDirection += -glm::cross(componentParent->GetDirection(), glm::vec3(0, 1, 0));
	if (glfwGetKey(window, GLFW_KEY_D))
		moveDirection += glm::cross(componentParent->GetDirection(), glm::vec3(0, 1, 0));

	Move(moveDirection, 0.03f);

	if (xoffset != 0 || yoffset != 0)
	{
		componentParent->SetEulerAngles(componentParent->GetEulerAngles() + glm::vec3(yoffset, xoffset, 0));
		xoffset = 0;
		yoffset = 0;
	}
}

void Controller::Move(glm::vec3 direction, float moveSpeed)
{
	if (direction != glm::vec3(0))
	{
		direction.y = 0;
		componentParent->Translate(glm::normalize(direction) * moveSpeed);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	
	xoffset = xpos - lastX;
	yoffset = lastY - ypos; // reversed sinxe y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	std::ostringstream ss;
	ss << "X: " << xoffset << ", Y: " << yoffset << "\n";
	printf(ss.str().c_str());
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	
	//yaw += xoffset;
	//pitch += yoffset;
	//
	//// make sure that when pitch is out of bounds, screen doesn't get flipped
	//
	//if (pitch > 89.0f)
	//	pitch = 89.0f;
	//if (pitch < -89.0f)
	//	pitch = -89.0f;
	
	//glm::vec3 front;
	//front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	//front.y = sin(glm::radians(pitch));
	//front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
}

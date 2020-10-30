#include <stdio.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <ModelManager.h>
#include "ManagerClasses/ObjectManager.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
#pragma region Setup

#pragma region GLFW Setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "ShooterGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#pragma endregion
#pragma region GLAD Setup
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
#pragma endregion

	ObjectManager* objectManager = new ObjectManager();
	objectManager->Initialize(window);
	glEnable(GL_DEPTH_TEST);

#pragma endregion
	float deltaTime = 0.0f; // Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	bool firstFrame = true;
#pragma region Gameplay Loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		float currentFrame = (float)glfwGetTime();
		deltaTime = !firstFrame * (currentFrame - lastFrame);
		firstFrame = false;
		glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		objectManager->Update(deltaTime);
		objectManager->Render();
		glfwSwapBuffers(window);
		glfwPollEvents();

		lastFrame = currentFrame;
		std::cout << "Last frametime = " << 1 / deltaTime << std::endl;
	}
#pragma endregion

#pragma region End game
	glfwTerminate();
	return 0;
#pragma endregion

}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that the width and
	// height will be significantly larger than specified on retina displays
	glViewport(0, 0, width, height);
}
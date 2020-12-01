#include "Controller.h"
#include "GLFW/glfw3.h"
#include "RigidBody.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
float lastX, lastY, xoffset, yoffset;
bool mouseButtonPressed0, mouseButtonDown0, mouseButtonReleased0;
bool mouseButtonPressed1, mouseButtonDown1, mouseButtonReleased1;
bool firstMouse;

void Controller::Initialize(GLFWwindow* newWindow)
{
	window = newWindow;
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	firstMouse = true;
}

void Controller::AddKeyBinding()
{
}

void Controller::Update(float gameTime)
{
#pragma region Update Keys/Inputs
	forwardKey.Update();
	leftKey.Update();
	downKey.Update();
	rightKey.Update();
	action1Key.Update();
	action2Key.Update();
	action3Key.Update();
	action4Key.Update();
	if (!isNetworked)
	{
		if (glfwGetKey(window, GLFW_KEY_W))
			forwardKey.Press();
		else
			forwardKey.Release();
		if (glfwGetKey(window, GLFW_KEY_A))
			leftKey.Press();
		else
			leftKey.Release();
		if (glfwGetKey(window, GLFW_KEY_S))
			downKey.Press();
		else
			downKey.Release();
		if (glfwGetKey(window, GLFW_KEY_D))
			rightKey.Press();
		else
			rightKey.Release();

		if (glfwGetKey(window, GLFW_KEY_U))
			action1Key.Press();
		else
			action1Key.Release();
		if (glfwGetKey(window, GLFW_KEY_I))
			action2Key.Press();
		else
			action2Key.Release();
		if (glfwGetKey(window, GLFW_KEY_O))
			action3Key.Press();
		else
			action3Key.Release();
		if (glfwGetKey(window, GLFW_KEY_P))
			action4Key.Press();
		else
			action4Key.Release();
	}
#pragma endregion
	CheckForMovement(forwardKey.IsDown(), leftKey.IsDown(), downKey.IsDown(), rightKey.IsDown(), gameTime);
	//Rotate the entity based on the mouse offset from the center of the screen
	if (xoffset != 0 || yoffset != 0)
	{
		componentParent->SetEulerAngles(componentParent->GetEulerAngles() + glm::vec3(-yoffset, xoffset, 0));
		xoffset = 0;
		yoffset = 0;

		//Controller can't make parent entity look above 89 degrees or below -89 degrees
		float pitch = componentParent->GetEulerAngles().x;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		componentParent->SetEulerAngles(glm::vec3(pitch, componentParent->GetEulerAngles().y, componentParent->GetEulerAngles().z));
	}

	mouseButtonPressed0 = false;
	mouseButtonPressed1 = false;
}

void Controller::Move(glm::vec3 direction, float moveSpeed, float gameTime)
{
	if (direction != glm::vec3(0))
	{
		RigidBody* rigidBody = componentParent->FindRigidBody();
		if (rigidBody == nullptr)
		{
			componentParent->Translate(glm::normalize(direction) * moveSpeed * gameTime);
		}
		else
		{
			//Don't multiply by gameTime because it is computed by the rigidBody->FixedUpdate function
			rigidBody->StoreVelocity(glm::normalize(direction) * moveSpeed);
		}
	}
	//If wasd input just changed and is not receiving directional input
	if (ChangedWASD() != 0 && CurrentWASD() == 0)
	{
		RigidBody* rigidBody = componentParent->FindRigidBody();
		if (rigidBody != nullptr)
		{
			rigidBody->SetVelocity(glm::vec3(0, 0, 0));
			rigidBody->StoreVelocity(-rigidBody->GetStoredVelocity());
		}
	}
}

int Controller::ChangedWASD()
{
	int forwardKeyChanged = WASDPacket::W & ((forwardKey.IsPressed() || forwardKey.IsReleased()) << 3);
	int leftKeyChanged = WASDPacket::A & ((leftKey.IsPressed() || leftKey.IsReleased()) << 2);
	int downKeyChanged = WASDPacket::S & ((downKey.IsPressed() || downKey.IsReleased()) << 1);
	int rightKeyChanged = WASDPacket::D & ((rightKey.IsPressed() || rightKey.IsReleased()) << 0);
	return forwardKeyChanged | leftKeyChanged | downKeyChanged | rightKeyChanged;
}

int Controller::CurrentWASD()
{
	int forwardKeyDown = forwardKey.IsDown() << 3;
	int leftKeyDown = leftKey.IsDown() << 2;
	int downKeyDown = downKey.IsDown() << 1;
	int rightKeyDown = rightKey.IsDown() << 0;
	return forwardKeyDown | leftKeyDown | downKeyDown | rightKeyDown;
}

int Controller::ChangedActionInput()
{
	int action4KeyChanged = WASDPacket::W & ((action4Key.IsPressed() || action4Key.IsReleased()) << 3);
	int action3KeyChanged = WASDPacket::A & ((action3Key.IsPressed() || action3Key.IsReleased()) << 2);
	int action2KeyChanged = WASDPacket::S & ((action2Key.IsPressed() || action2Key.IsReleased()) << 1);
	int action1KeyChanged = WASDPacket::D & ((action1Key.IsPressed() || action1Key.IsReleased()) << 0);
	return action4KeyChanged | action3KeyChanged | action2KeyChanged | action1KeyChanged;
}

int Controller::CurrentActionInput()
{
	int action4KeyDown = action4Key.IsDown() << 3;
	int action3KeyDown = action3Key.IsDown() << 2;
	int action2KeyDown = action2Key.IsDown() << 1;
	int action1KeyDown = action1Key.IsDown() << 0;
	return action4KeyDown | action3KeyDown | action2KeyDown | action1KeyDown;
}

void Controller::SetPlayerID(unsigned int newPlayerID)
{
	playerID = newPlayerID;
}

unsigned int Controller::GetPlayerID()
{
	return playerID;
}

void Controller::SetNetworkWASDInput(int wasd)
{
	if (wasd & WASDPacket::W)
		forwardKey.Press();
	else
		forwardKey.Release();
	if (wasd & WASDPacket::A)
		leftKey.Press();
	else
		leftKey.Release();
	if (wasd & WASDPacket::S)
		downKey.Press();
	else
		downKey.Release();
	if (wasd & WASDPacket::D)
		rightKey.Press();
	else
		rightKey.Release();

	//If wasd input just changed and is not receiving directional input
	if (CurrentWASD() == 0)
	{
		RigidBody* rigidBody = componentParent->FindRigidBody();
		if (rigidBody != nullptr)
		{
			rigidBody->SetVelocity(glm::vec3(0, 0, 0));
			rigidBody->StoreVelocity(-rigidBody->GetStoredVelocity());
		}
	}
}

void Controller::SetNetworkActionInput(int action)
{

	if (action & ActionPacket::Action4)
		action4Key.Press();
	else
		action4Key.Release();
	if (action & ActionPacket::Action3)
		action3Key.Press();
	else
		action3Key.Release();
	if (action & ActionPacket::Action2)
		action2Key.Press();
	else
		action2Key.Release();
	if (action & ActionPacket::Action1)
		action1Key.Press();
	else
		action1Key.Release();
}

void Controller::SetIsNetworked(bool in_isNetworked)
{
	isNetworked = in_isNetworked;
}

bool Controller::GetIsNetworked()
{
	return isNetworked;
}

void Controller::CheckForMovement(bool wKey, bool aKey, bool sKey, bool dKey, float gameTime)
{
	glm::vec3 moveDirection = glm::vec3(0);
	if (wKey)
		moveDirection -= componentParent->GetTargetDirection();
	if (sKey)
		moveDirection += componentParent->GetTargetDirection();
	if (aKey)
		moveDirection += glm::cross(componentParent->GetTargetDirection(), glm::vec3(0, 1, 0));
	if (dKey)
		moveDirection -= glm::cross(componentParent->GetTargetDirection(), glm::vec3(0, 1, 0));

	//Keeps the player from moving up/down
	moveDirection.y = 0;
	if (!isNetworked)
	{
		//Moves the player up or down
		if (mouseButtonDown0)
			moveDirection += glm::vec3(0, 0.001, 0);
		if (mouseButtonDown1)
			moveDirection -= glm::vec3(0, 0.001, 0);
	}
	Move(moveDirection, -3.f, gameTime);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		mouseButtonDown1 = true;
		mouseButtonPressed1 = true;
		mouseButtonReleased1 = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		mouseButtonReleased1 = true;
		mouseButtonDown1 = false;
		mouseButtonPressed1 = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		mouseButtonDown0 = true;
		mouseButtonPressed0 = true;
		mouseButtonReleased0 = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseButtonReleased0 = true;
		mouseButtonDown0 = false;
		mouseButtonPressed0 = false;
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
	
	xoffset = -(lastX - xpos);
	yoffset = (lastY - ypos);
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;	
}

#pragma once
#include "Component.h"
#include "enet/enet.h"
struct GLFWwindow;

class Controller : public Component
{
	enum WASDPacket
	{
		D = (1 << 0),
		S = (1 << 1),
		A = (1 << 2),
		W = (1 << 3)
	};
	//TODO: Add hot swappable controls
	//Hot swapping for controls 
public:
	void Initialize(GLFWwindow* newWindow);
	void AddKeyBinding(/*Function to call*/);
	virtual void Update(float gameTime) override;
	//TODO: Add keyboard/controller support
	//TODO: Add support for different entity states. These should be hot swappable
	void Move(glm::vec3 direction, float moveSpeed);
	//void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void SetPlayerID(unsigned int newPlayerID);
	unsigned int GetPlayerID();

	void GetNetworkInput(int wasd);

	void SetIsNetworked(bool in_isNetworked);
	bool GetIsNetworked();
private:
	void CheckForMovement(bool wKey, bool aKey, bool sKey, bool dKey, float gameTime);
private:
	GLFWwindow* window;
	bool isNetworked;
	bool forwardKeyPressed, leftKeyPressed, downKeyPressed, rightKeyPressed;
	unsigned int playerID;
};
#pragma once
#include "Component.h"
#include "enet/enet.h"
struct GLFWwindow;

struct KeyData
{
private:
	bool isDown = false;
	bool wasDown = false;
public:
	bool IsPressed() { return isDown && !wasDown; }
	bool IsReleased() { return !isDown && wasDown; }
	bool IsDown() { return isDown; }
	void Press() { isDown = true; }
	void Release() { isDown = false; }
	void Update() { wasDown = isDown; }
};

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
	
	//The value of the change in WASD this frame
	int ChangedWASD();
	//The value of the current WASD this frame
	int CurrentWASD();

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
	KeyData forwardKey, leftKey, downKey, rightKey;
	unsigned int playerID = -1;
};
#pragma once
#include "BaseManager.h"

class AIManager : public BaseManager
{
	enum AIState { idle, patrol, suspicious, alert, attack, defend, retreat };
	AIState aiState;

	Entity* target;

	void Jump();
	void Walk();
	void Run();
	void Attack();

	void Update(float gameTime);
};
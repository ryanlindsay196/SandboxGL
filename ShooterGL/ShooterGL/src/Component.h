#pragma once
#include "Entity.h"

class Component
{
public:
	Entity* componentParent;

	void Update(float gameTime);
};
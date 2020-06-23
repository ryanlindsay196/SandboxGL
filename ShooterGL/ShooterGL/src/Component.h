#pragma once
#include "Entity.h"

class Component
{
public:
	Entity* componentParent;

	virtual void Update(float gameTime);
	virtual void Render() {};
};
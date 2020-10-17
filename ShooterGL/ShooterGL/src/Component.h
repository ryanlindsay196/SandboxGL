#pragma once
#include "Entity.h"
class LightManager;

class Component
{
public:
	Entity* componentParent;

	virtual void Update(float gameTime);
	virtual void Render(LightManager* lightManager) {};
};
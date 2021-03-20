#pragma once
#include "Entity.h"
class LightManager;

class Component
{
	//Index that this Component has componentParent.components.
	unsigned int entityComponentIndex;
public:
	Entity* componentParent;

	virtual void Update(float gameTime);
	virtual void Render(LightManager* lightManager) {};

	virtual void OnCollisionEnter(Entity* entity);
	virtual void OnTriggerEnter(Entity* entity);

	virtual void MoveChildReferences() {};
public:
	unsigned int GetEntityComponentIndex();
	void SetEntityComponentIndex(unsigned int index);
};
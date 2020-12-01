#pragma once
#include "glm.hpp"
class RigidBody;

class Collider
{
public:
	void Update(float gameTime);

	enum ColliderType { Sphere, Rectangle, Mesh };

	RigidBody* colliderParent;
	bool isTrigger = false;
	bool isActive = true;
	ColliderType colliderType;
	glm::vec3 positionOffset, scale;

	//The projections of the bounds of a rigidbody on the x, y, and z axes.
	struct ColliderProjections
	{
		float x[2];
		float y[2];
		float z[2];
	};
	ColliderProjections CalculateProjections(bool addVelocity, bool addStoredVelocity, float gameTime = 1);

private:
};
#pragma once
#include "Component.h"
#include <algorithm>
#include <string>
class RigidBody : public Component
{
public:
	enum ColliderType { Sphere, Rectangle, Mesh };
private:
	//TODO: Make fixedUpdateTime a globally set variable. Maybe set it in a ProjectSettings file?
	//float fixedUpdateTime, fixedUpdateTimer;
	float mass;
	glm::vec3 velocity;
	glm::vec3 positionConstraints, rotationConstraints;
	glm::vec3 positionOffset, scale;
	bool useGravity;

	ColliderType colliderType;
public:
	RigidBody();
	~RigidBody();
	void Initialize(std::vector<std::string>& rigidBodyProperties);
	void Update(float gameTime) override;
	void FixedUpdate(float gameTime);
	ColliderType GetColliderType();
	glm::vec3 GetPositionOffset();
	glm::vec3 GetScale();
	glm::vec3 GetVelocity();

	void SetVelocity(glm::vec3 newVelocity);

	//The projections of the bounds of a rigidbody on the x, y, and z axes.
	struct RigidBodyProjections
	{
		float xVec3Projections[2];
		float yVec3Projections[2];
		float zVec3Projections[2];
	};
	RigidBodyProjections CalculateProjections();
};


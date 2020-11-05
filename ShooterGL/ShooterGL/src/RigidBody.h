#pragma once
#include "Component.h"
#include <algorithm>
#include <string>
class RigidBody : public Component
{
public:
	enum ColliderType { Sphere, Rectangle, Mesh };
private:
	float mass;
	glm::vec3 velocity, storedVelocity;
	glm::vec3 positionConstraints, rotationConstraints;
	glm::vec3 positionOffset, scale;
	bool useGravity;
	bool isTrigger = false;

	bool isVelocityStored;

	ColliderType colliderType;
public:
	RigidBody();
	~RigidBody();
	void Initialize(std::vector<std::string>& rigidBodyProperties);
	void Update(float gameTime) override;
	void FixedUpdate(float gameTime);
	ColliderType GetColliderType();
	glm::vec3 GetPositionOffset();
	glm::vec3 GetPosition();
	glm::vec3 GetScale();
	glm::vec3 GetVelocity();
	float GetMass();
	float GetMomentumFloat();
	glm::vec3 GetMomentumVec3();
	float GetStoredMomentum();
	void StoreVelocity(glm::vec3 velocityToAdd);

	void SetVelocity(glm::vec3 newVelocity);

	void OnCollisionEnter(Entity* entity);
	void OnTriggerEnter(Entity* entity);

public:
	bool IsTrigger();

	//The projections of the bounds of a rigidbody on the x, y, and z axes.
	struct RigidBodyProjections
	{
		float x[2];
		float y[2];
		float z[2];
	};
	RigidBodyProjections CalculateProjections(bool addVelocity, bool addStoredVelocity, float gameTime = 1);


	//TODO: Remove
	int tempIndex;
};


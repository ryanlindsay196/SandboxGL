#pragma once
#include "Component.h"
#include <algorithm>
#include <string>
#include <Collider.h>
class RigidBody : public Component
{
private:
	float mass;
	glm::vec3 velocity, storedVelocity;
	glm::vec3 positionConstraints, rotationConstraints;
	bool useGravity;

	bool isVelocityStored;
	std::vector<Collider> colliders;
public:
	RigidBody();
	~RigidBody();
	void Initialize(std::vector<std::string>& rigidBodyProperties);
	void Update(float gameTime) override;
	void FixedUpdate(float gameTime);
	//glm::vec3 GetPositionOffset();
	glm::vec3 GetPosition();
	//glm::vec3 GetScale();
	glm::vec3 GetVelocity();
	glm::vec3 GetStoredVelocity();
	float GetMass();
	float GetMomentumFloat();
	glm::vec3 GetMomentumVec3();
	float GetStoredMomentum();
	void StoreVelocity(glm::vec3 velocityToAdd);

	void SetVelocity(glm::vec3 newVelocity);

	void OnCollisionEnter(Entity* entity);
	void OnTriggerEnter(Entity* entity);

public:
	std::vector<Collider> GetColliders();
	Collider* GetColliderRef(int i);
	//TODO: Remove
	//int tempIndex;
};


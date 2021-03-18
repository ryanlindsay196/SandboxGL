#pragma once
#include "Component.h"
#include <algorithm>
#include <string>
#include <Collider.h>
class RigidBody : public Component
{
public:
	enum Properties
	{
		HitBox = 1,
		Static = 1 << 1,
		DestroyOnHit = 1 << 2,
		ReactivateTimerActive = 1 << 3
	};
private:
	unsigned int properties;
	float mass, bounce;
	glm::vec3 velocity, storedVelocity;
	glm::vec3 positionConstraints, rotationConstraints;
	bool useGravity;
	Entity* spawnedBy;

	bool isActive = true;
	bool isVelocityStored;
	std::vector<Collider> colliders;

	float reactivateTimer;
public:
	RigidBody();
	~RigidBody();
	void Initialize(std::vector<std::string>& rigidBodyProperties, Entity* spawningEntity);
	void Update(float gameTime) override;
	void FixedUpdate(float gameTime);
	void SetSpawnedBy(Entity* spawningEntity);
	Entity* GetSpawnedBy();
	//glm::vec3 GetPositionOffset();
	glm::vec3 GetPosition();
	//glm::vec3 GetScale();
	glm::vec3 GetVelocity();
	glm::vec3 GetStoredVelocity();
	glm::vec3 GetPositionConstraints();
	float GetBounce();
	float GetMass();
	float GetMomentumFloat();
	glm::vec3 GetMomentumVec3();
	float GetStoredMomentum();

	void ResetStoredVelocity();
	void StoreVelocity(glm::vec3 velocityToAdd);

	void SetVelocity(glm::vec3 newVelocity);

	void OnCollisionEnter(Entity* entity);
	void OnTriggerEnter(Entity* entity);

	std::vector<Collider> GetColliders();
	Collider* GetColliderRef(int i);

	void SetIsActive(bool newIsActive);
	bool GetIsActive();

	void SetReactivateTimer(float newReactivateTimer);
	unsigned int GetProperties();
	void SetProperties(int i);
public:
};


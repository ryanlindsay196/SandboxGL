#pragma once
#include <vector>
#include "glm.hpp"
#include "RigidBody.h"
#include <iostream>
#include <algorithm>
#include "BaseManager.h"

//Node in a linked list.
struct RigidBodyNode
{
	//Index of the rigidBody inside the rigidBodies list
	unsigned int rigidBodyIndex;
	//Pointer to the next node
	RigidBodyNode* nextNode;
};

//If a rigidbody and physics region have their bounds overlap, the rigidbody is added to the physics region
struct PhysicsRegion
{
	//Starting bound for the region
	glm::vec3 startBound;
	//Ending bound for the region
	glm::vec3 endBound;
	//First rigidbody inside the region
	RigidBodyNode* startNode;
};

class PhysicsManager : public BaseManager
{
private:
	static PhysicsManager* instance;
	PhysicsManager();
	PhysicsManager(PhysicsManager&) = delete;

	float CalculateCollisionTime(const glm::vec3 startPos, const glm::vec3 endPos, const glm::vec3 startVelocity, const glm::vec3 acceleration, const float maxTimeStep, const float timeStep, RigidBody& rb1, RigidBody& rb2);

public:
	static inline glm::vec3 CalculateDisplacement(const glm::vec3 startVelocity, const glm::vec3 acceleration, const float gameTime) {
		return (startVelocity * gameTime) + (acceleration * 0.5f * gameTime * gameTime);
	};
	static inline glm::vec3 CalculateVelocity(const glm::vec3 startVelocity, const glm::vec3 acceleration, const float gameTime) { return startVelocity + (acceleration * gameTime); };

	static inline glm::vec3 CalculateVelocity(const glm::vec3 startVelocity, const glm::vec3 acceleration, const glm::vec3 displacement) {
		glm::vec3 finalVelocitySquared = (startVelocity * startVelocity) + (acceleration * displacement * 2.f);
		return glm::vec3(sqrtf(finalVelocitySquared.x), sqrtf(finalVelocitySquared.y), sqrtf(finalVelocitySquared.z));
	}
	;

private:
	//set this in initialize
	glm::vec3 physicsRegionBounds;
	//The total number of physics regions in the scene
	glm::vec3 totalPhysicsRegions;
	//List of rigid bodies
	std::vector<RigidBody> rigidBodies;
	//initialize based on physicsRegionBounds
	std::vector<std::vector<std::vector<PhysicsRegion>>> physicsRegions;
	PhysicsRegion outerPhysicsRegion;

	//Moves through all regions, removing rigidbodies that leave a region
	void UpdatePhysicsRegions_RemoveNodes(float gameTime);

	//Moves through all retions, adding rigidbodies that enter a region
	void UpdatePhysicsRegions_AddNodes(RigidBody* rb, float gameTime, unsigned int rigidBodyIndex);

	//Iterate through all rigidbodies (grouped by region), and check if they're colliding
	void CheckCollisions(int iterations, float gameTime);
	//Check for collisions between all objects inside a given region
	void CheckCollisionsInRegion(PhysicsRegion physicsRegion, float gameTime);
	//Returns true if the two colliders are overlapping
	bool IsColliding(Collider * collider1, Collider * collider2, float gameTime, glm::vec3& normalDirection);

	//checks if a rigidbody is inside a particular region
	bool RigidBodyInRegion(unsigned int rbIndex, PhysicsRegion& currentPhysicsRegion, bool addRB_Velocity, float gameTime);
public:
	static PhysicsManager* GetInstance();

	~PhysicsManager();
	void Initialize(glm::vec3 regionBounds, glm::vec3 regionCount);
	void FixedUpdate(float gameTime);

	//Add rigidbody to the rigidbody list and add it to one or more physics regions
	RigidBody* InitializeRigidBody(Entity* entity, float gameTime, std::vector<std::string> rigidBodyProperties, unsigned int newEntityComponentIndex);
	//Replace the starting node in a rigidbody region with a new one
	void AddRigidBodyToRegion(unsigned int rbIndex, glm::vec3 region);
public:
	static inline glm::vec3 Acceleration() { return glm::vec3(0, -1, 0); }

};


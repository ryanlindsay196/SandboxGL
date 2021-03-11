#pragma once
#include <vector>
#include "glm.hpp"
#include "RigidBody.h"
#include <iostream>
#include <algorithm>

//Node in a linked list.
struct RigidBodyNode
{
	//Pointer to a rigidbody in the rigidBodies list
	RigidBody* rigidBody;
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

class PhysicsManager
{
private:
	static PhysicsManager* instance;
	PhysicsManager();

private:
	//set this in initialize
	glm::vec3 physicsRegionBounds;
	//The total number of physics regions in the scene
	glm::vec3 totalPhysicsRegions;
	//update this whenever a new rigidbody is instantiated
	std::vector<RigidBody*> rigidBodies;
	//initialize based on physicsRegionBounds
	std::vector<std::vector<std::vector<PhysicsRegion>>> physicsRegions;
	PhysicsRegion outerPhysicsRegion;

	//Moves through all regions, removing rigidbodies that leave a region
	void UpdatePhysicsRegions_RemoveNodes(float gameTime);

	//Moves through all retions, adding rigidbodies that enter a region
	void UpdatePhysicsRegions_AddNodes(RigidBody* rb, float gameTime);

	//Iterate through all rigidbodies (grouped by region), and check if they're colliding
	void CheckCollisions(int iterations, float gameTime);
	//Check for collisions between all objects inside a given region
	void CheckCollisionsInRegion(PhysicsRegion physicsRegion, float gameTime);
	//Returns true if the two colliders are overlapping
	bool IsColliding(Collider * collider1, Collider * collider2, float gameTime, glm::vec3& normalDirection);

	//checks if a rigidbody is inside a particular region
	bool RigidBodyInRegion(RigidBody* rb, PhysicsRegion& currentPhysicsRegion, bool addRB_Velocity, float gameTime);
public:
	static PhysicsManager* GetInstance();

	~PhysicsManager();
	void Initialize(glm::vec3 regionBounds, glm::vec3 regionCount);
	void FixedUpdate(float gameTime);

	//Add rigidbody to the rigidbody list and add it to one or more physics regions
	void InitializeRigidBody(RigidBody* rb, float gameTime);
	//Replace the starting node in a rigidbody region with a new one
	void AddRigidBodyToRegion(RigidBody* rb, glm::vec3 region);
};


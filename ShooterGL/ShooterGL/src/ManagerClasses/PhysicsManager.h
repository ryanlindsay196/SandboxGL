#pragma once
#include <vector>
#include "glm.hpp"
#include "RigidBody.h"
#include <iostream>
#include <algorithm>

struct RigidBodyNode
{
	RigidBody* rigidBody;
	RigidBodyNode* nextNode;
};

struct PhysicsRegion
{
	glm::vec3 startBound;
	glm::vec3 endBound;
	RigidBodyNode* startNode;
};

class PhysicsManager
{
private:
	//set this in initialize
	glm::vec3 physicsRegionBounds;
	//update this whenever a new rigidbody is instantiated
	std::vector<RigidBody*> rigidBodies;
	//initialize based on physicsRegionBounds
	std::vector<std::vector<std::vector<PhysicsRegion>>> physicsRegions;

	//Moves through all regions, removing rigidbodies that leave a region
	void UpdatePhysicsRegions_RemoveNodes(float gameTime);

	//Moves through all retions, adding rigidbodies that enter a region
	void UpdatePhysicsRegions_AddNodes(RigidBody* rb, float gameTime);


	void CheckCollisions(int iterations, float gameTime);
	bool IsColliding(RigidBody::RigidBodyProjections rbProjections1, RigidBody::RigidBodyProjections rbProjections2);

	//checks if a rigidbody is inside a particular region
	bool RigidBodyInRegion(RigidBody* rb, glm::vec3 regionIDs, bool addRB_Velocity, float gameTime);
public:
	PhysicsManager();
	~PhysicsManager();
	void Initialize(glm::vec3 regionBounds, glm::vec3 regionCount);
	void FixedUpdate(float gameTime);

	void InitializeRigidBody(RigidBody* rb, float gameTime);
	void AddRigidBodyToRegion(RigidBody* rb, glm::vec3 region);
};


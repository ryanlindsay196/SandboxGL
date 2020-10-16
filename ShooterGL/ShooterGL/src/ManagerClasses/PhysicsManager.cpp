#include "PhysicsManager.h"



PhysicsManager::PhysicsManager()
{
}


PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::Initialize(glm::vec3 regionBounds, glm::vec3 regionCount)
{
	physicsRegionBounds = regionBounds;
	for (int i = 0; i < regionCount.x; i++)
	{
		physicsRegions.push_back(std::vector<std::vector<PhysicsRegion>>());
		for (int j = 0; j < regionCount.y; j++)
		{
			physicsRegions[i].push_back(std::vector<PhysicsRegion>());
			for (int k = 0; k < regionCount.z; k++)
			{
				physicsRegions[i][j].push_back(PhysicsRegion());
				physicsRegions[i][j][k].startBound = glm::vec3(i * regionBounds.x, j * regionBounds.y, k * regionBounds.z);
				physicsRegions[i][j][k].endBound = glm::vec3((i + 1) * regionBounds.x, (j + 1) * regionBounds.y, (k + 1) * regionBounds.z);
			}
		}
	}
}

void PhysicsManager::FixedUpdate(float gameTime)
{
	UpdatePhysicsRegions_RemoveNodes(gameTime);
	for (int i = 0; i < rigidBodies.size(); i++)
	{
		UpdatePhysicsRegions_AddNodes(rigidBodies[i], gameTime);
		rigidBodies[i]->FixedUpdate(gameTime);
		CheckCollisions(8);
	}
}

//Loop through all regions. Add nodes to a region if they entered the region this frame
void PhysicsManager::UpdatePhysicsRegions_AddNodes(RigidBody * rb, float gameTime)
{
	glm::vec3 physicsRegionToAdd = (rb->componentParent->GetTranslation() + rb->GetPositionOffset()) / physicsRegionBounds;
	//Make sure the physics region to add to is an integer and not a float
	physicsRegionToAdd = glm::vec3(floorf(physicsRegionToAdd.x), floorf(physicsRegionToAdd.y), floorf(physicsRegionToAdd.z));

	glm::vec3 physicsRegionRangeToCheck;
	//Loop through all potential physics region to add the current rigidbody to.
	//Each for statement checks that the physics region indices are inside the bounds of physicsRegions
	//I add a rigidbody to a physics region if it just entered (current position is not overlapping region, but position + velocity is overlapping region)
	for (physicsRegionRangeToCheck.x = std::max((int)physicsRegionToAdd.x - 1, 0); physicsRegionRangeToCheck.x <= std::min((int)physicsRegionToAdd.x + 1, (int)physicsRegions.size() - 1); physicsRegionRangeToCheck.x++)
	{
		for (physicsRegionRangeToCheck.y = std::max((int)physicsRegionToAdd.y - 1, 0); physicsRegionRangeToCheck.y <= std::min((int)physicsRegionToAdd.y + 1, (int)physicsRegions[physicsRegionRangeToCheck.x].size() - 1); physicsRegionRangeToCheck.y++)
		{
			for (physicsRegionRangeToCheck.z = std::max((int)physicsRegionToAdd.z - 1, 0); physicsRegionRangeToCheck.z <= std::min((int)physicsRegionToAdd.z + 1, (int)physicsRegions[physicsRegionRangeToCheck.x][physicsRegionRangeToCheck.y].size() - 1); physicsRegionRangeToCheck.z++)
			{
				{
					//If rigidbody is just entering a new physics region (position + velocity is inside the region, but position is not)
					if (!RigidBodyInRegion(rb, physicsRegionRangeToCheck, false, gameTime) && RigidBodyInRegion(rb, physicsRegionRangeToCheck, true, gameTime))
					{
						//add it to the region
						AddRigidBodyToRegion(rb, physicsRegionRangeToCheck);
						std::cout << "Added rigidBody: " << rb << " to physics region " << physicsRegionRangeToCheck.x << ", " << physicsRegionRangeToCheck.y << ", " << physicsRegionRangeToCheck.z << std::endl;
					}
				}
			}
		}
	}
}

//When adding a rigidbody to a scene (during loading or instantiation)
void PhysicsManager::InitializeRigidBody(RigidBody * rb, float gameTime)
{
	rigidBodies.push_back(rb);

	//TODO: Remove
	if(rigidBodies.size() == 1)
		rigidBodies[0]->SetVelocity(glm::vec3(-1, 0, 0));
	else if(rigidBodies.size() == 2)
		rigidBodies[1]->SetVelocity(glm::vec3(1, 0, 0));

	glm::vec3 physicsRegionToAdd = (rb->componentParent->GetTranslation() + rb->GetPositionOffset()) / physicsRegionBounds;
	//Make sure the physics region to add to is an integer and not a float
	physicsRegionToAdd = glm::vec3(floorf(physicsRegionToAdd.x), floorf(physicsRegionToAdd.y), floorf(physicsRegionToAdd.z));

	glm::vec3 physicsRegionRangeToCheck;
	//Loop through all potential physics region to add the current rigidbody to.
	//Each for statement checks that the physics region indices are inside the bounds of physicsRegions
	for (physicsRegionRangeToCheck.x = std::max((int)physicsRegionToAdd.x - 1, 0); physicsRegionRangeToCheck.x <= std::min((int)physicsRegionToAdd.x + 1, (int)physicsRegions.size() - 1); physicsRegionRangeToCheck.x++)
	{
		for (physicsRegionRangeToCheck.y = std::max((int)physicsRegionToAdd.y - 1, 0); physicsRegionRangeToCheck.y <= std::min((int)physicsRegionToAdd.y + 1, (int)physicsRegions[physicsRegionRangeToCheck.x].size() - 1); physicsRegionRangeToCheck.y++)
		{
			for (physicsRegionRangeToCheck.z = std::max((int)physicsRegionToAdd.z - 1, 0); physicsRegionRangeToCheck.z <= std::min((int)physicsRegionToAdd.z + 1, (int)physicsRegions[physicsRegionRangeToCheck.x][physicsRegionRangeToCheck.y].size() - 1); physicsRegionRangeToCheck.z++)
			{
				//If rigidbody is in the physics region
				if (RigidBodyInRegion(rb, physicsRegionRangeToCheck, false, gameTime))
				{
					//add it to the region
					AddRigidBodyToRegion(rb, physicsRegionRangeToCheck);
					std::cout << "Added rigidBody: " << rb << " to physics region " << physicsRegionRangeToCheck.x << ", " << physicsRegionRangeToCheck.y << ", " << physicsRegionRangeToCheck.z << std::endl;
				}
			}
		}
	}
}

void PhysicsManager::AddRigidBodyToRegion(RigidBody * rb, glm::vec3 region)
{
	RigidBodyNode* oldStartNode = physicsRegions[region.x][region.y][region.z].startNode;
	physicsRegions[region.x][region.y][region.z].startNode = new RigidBodyNode();
	physicsRegions[region.x][region.y][region.z].startNode->rigidBody = rb;
	physicsRegions[region.x][region.y][region.z].startNode->nextNode = oldStartNode;
}

//Loops through all regions. If a node is no longer in a particular region, remove the node from the region.
void PhysicsManager::UpdatePhysicsRegions_RemoveNodes(float gameTime)
{
	glm::vec3 regionIDs;
	RigidBodyNode* prevRBNode = nullptr;
	for (regionIDs.x = 0; regionIDs.x < physicsRegions.size(); regionIDs.x++)
	{
		for (regionIDs.y = 0; regionIDs.y < physicsRegions[regionIDs.x].size(); regionIDs.y++)
		{
			for (regionIDs.z = 0; regionIDs.z < physicsRegions[regionIDs.x][regionIDs.y].size(); regionIDs.z++)
			{
				RigidBodyNode* rbNode = physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startNode;
				while (rbNode != nullptr)
				{
					//If the rigidBody isn't inside the region
					if (!RigidBodyInRegion(rbNode->rigidBody, regionIDs, false, gameTime) && !RigidBodyInRegion(rbNode->rigidBody, regionIDs, true, gameTime))
					{
						std::cout << "Removed rigidBody: " << rbNode->rigidBody << " from physics region " << regionIDs.x << ", " << regionIDs.y << ", " << regionIDs.z << std::endl;
						//If we're not at the head node for the physics region
						if (prevRBNode != nullptr && rbNode != physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startNode)
						{
							//Remove current Node
							prevRBNode->nextNode = rbNode->nextNode;
							delete(rbNode);
							rbNode = prevRBNode->nextNode;
						}
						else
							//If we're at the head node for the physics region
						{
							physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startNode = rbNode->nextNode;
							delete(rbNode);
							rbNode = physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startNode;
						}
					}
					else
					{
						prevRBNode = rbNode;
						rbNode = rbNode->nextNode;
					}
				}
			}
		}
	}
}

//Check what collisions are present and handle whatever happens to collided objects
void PhysicsManager::CheckCollisions(int iterations)
{
	for(int currentIteration = 0; currentIteration < iterations; currentIteration++)
	{
		for (int i = 0; i < physicsRegions.size(); i++)
		{
			for (int j = 0; j < physicsRegions[i].size(); j++)
			{
				for (int k = 0; k < physicsRegions[i][j].size(); k++)
				{
					if (physicsRegions[i][j][k].startNode == nullptr)
						continue;
					RigidBodyNode* rbNode1 = physicsRegions[i][j][k].startNode;
					RigidBodyNode* rbNode2 = physicsRegions[i][j][k].startNode->nextNode;
					if (rbNode2 == nullptr)
						continue;
					while (rbNode1->nextNode != nullptr)
					{
						if (rbNode1 != rbNode2)
						{
							//TODO: Check the collision
							if(IsColliding(rbNode1->rigidBody->CalculateProjections(), rbNode2->rigidBody->CalculateProjections()))
							{
								std::cout << "RigidBody " << rbNode1->rigidBody << " has collided with " << rbNode2->rigidBody << "!" << std::endl;
								//rbNode1->rigidBody->SetVelocity(glm::vec3(0,1,0));
								//rbNode2->rigidBody->SetVelocity(glm::vec3(0,-1,0));
								
								glm::vec3 velocity1 = rbNode1->rigidBody->GetVelocity();
								glm::vec3 velocity2 = rbNode2->rigidBody->GetVelocity();
								float mass1 = rbNode1->rigidBody->GetMass();
								float mass2 = rbNode2->rigidBody->GetMass();

								rbNode1->rigidBody->StoreVelocity((velocity1 * (mass1 - mass2) / (mass2 + mass1)) + ((2 * mass2 * velocity2) / (mass2 + mass1)));
								rbNode2->rigidBody->StoreVelocity(((velocity1 * 2.f * mass1) / (mass2 + mass1)) + (velocity2 * (mass2 - mass1) / (mass2 + mass1)));
							}
						}
						rbNode2 = rbNode2->nextNode;
						//if node2 has reached the end, increment node1 and loop again
						if (rbNode2 == nullptr)
						{
							rbNode1 = rbNode1->nextNode;
							rbNode2 = rbNode1->nextNode;
						}
					}
				}
			}
		}
	}
}

//Check for collisions using vector projections on the x, y, and z axes.
bool PhysicsManager::IsColliding(RigidBody::RigidBodyProjections rbProjections1, RigidBody::RigidBodyProjections rbProjections2)
{
	//TODO: This works with box colliders, but not sphere colliders. Fix that.
	//if ((rbProjections1.xVec3Projections[0] < rbProjections2.xVec3Projections[0] || rbProjections1.xVec3Projections[0] > rbProjections2.xVec3Projections[1]) &&
	//	(rbProjections1.xVec3Projections[1] < rbProjections2.xVec3Projections[0] || rbProjections1.xVec3Projections[1] > rbProjections2.xVec3Projections[1]))
	//{
	//	if ((rbProjections2.xVec3Projections[0] < rbProjections1.xVec3Projections[0] || rbProjections2.xVec3Projections[0] > rbProjections1.xVec3Projections[1]) &&
	//		(rbProjections2.xVec3Projections[1] < rbProjections1.xVec3Projections[0] || rbProjections2.xVec3Projections[1] > rbProjections1.xVec3Projections[1]))
	//	{
	//		return false;
	//	}
	//}
	if (rbProjections1.x[0] > rbProjections2.x[1] || rbProjections2.x[0] > rbProjections1.x[1])
		return false;
	if (rbProjections1.y[0] > rbProjections2.y[1] || rbProjections2.y[0] > rbProjections1.y[1])
		return false;
	if (rbProjections1.z[0] > rbProjections2.z[1] || rbProjections2.z[0] > rbProjections1.z[1])
		return false;
	return true;
}

bool PhysicsManager::RigidBodyInRegion(RigidBody * rb, glm::vec3 regionIDs, bool addRB_Velocity, float gameTime)
{
	const glm::vec3 rbVelocityConstant = rb->GetVelocity() * gameTime;
	//rectangle collider
	if (rb->GetColliderType() == RigidBody::ColliderType::Rectangle)
	{
		float rb_xPlusEdge = rb->componentParent->GetTranslation().x + (rb->GetPositionOffset().x + rb->GetScale().x);
		float rb_xMinusEdge = rb->componentParent->GetTranslation().x + (rb->GetPositionOffset().x - rb->GetScale().x);
		float rb_yPlusEdge = rb->componentParent->GetTranslation().y + (rb->GetPositionOffset().y + rb->GetScale().y);
		float rb_yMinusEdge = rb->componentParent->GetTranslation().y + (rb->GetPositionOffset().y - rb->GetScale().y);
		float rb_zPlusEdge = rb->componentParent->GetTranslation().z + (rb->GetPositionOffset().z + rb->GetScale().z);
		float rb_zMinusEdge = rb->componentParent->GetTranslation().z - (rb->GetPositionOffset().z + rb->GetScale().z);

		if (addRB_Velocity)
		{
			rb_xPlusEdge += rbVelocityConstant.x;
			rb_xMinusEdge += rbVelocityConstant.x;
			rb_yPlusEdge += rbVelocityConstant.y;
			rb_yMinusEdge += rbVelocityConstant.y;
			rb_zPlusEdge += rbVelocityConstant.z;
			rb_zMinusEdge += rbVelocityConstant.z;
		}

		//if one of the x-axis aligned edges of the cube are outside the physics region
		if (!(rb_xPlusEdge >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.x &&
			rb_xPlusEdge <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.x) &&
			!(rb_xMinusEdge >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.x &&
				rb_xMinusEdge <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.x))
		{
			return false;
		}
		//if one of the y-axis aligned edges of the cube are outside the physics region
		if (!(rb_yPlusEdge >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.y &&
			rb_yPlusEdge <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.y) &&
			!(rb_yMinusEdge >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.y &&
				rb_yMinusEdge <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.y))
		{
			return false;
		}
		//if one of the z-axis aligned edges of the cube are outside the physics region
		if (!(rb_zPlusEdge >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.z &&
			rb_zPlusEdge <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.z) &&
			!(rb_zMinusEdge >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.z &&
				rb_zMinusEdge <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.z))
		{
			return false;
		}
		return true;
	}

	//sphere collider
	else if (rb->GetColliderType() == RigidBody::ColliderType::Sphere)
	{
		glm::vec3 rigidBodyCenter = rb->componentParent->GetTranslation() + rb->GetPositionOffset();
		glm::vec3 regionCenter = (physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound + physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound) / glm::vec3(2);
		glm::vec3 closestSpherePoint = rigidBodyCenter + (normalize(regionCenter - rigidBodyCenter) * rb->GetScale());

		if (addRB_Velocity)
		{
			closestSpherePoint += rbVelocityConstant;
		}

		if (closestSpherePoint.x >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.x &&
			closestSpherePoint.x <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.x &&
			closestSpherePoint.y >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.y &&
			closestSpherePoint.y <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.y &&
			closestSpherePoint.z >= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startBound.z &&
			closestSpherePoint.z <= physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].endBound.z)
			return true;
	}

	//mesh collider
	//TODO: Implement mesh collider
	else if (rb->GetColliderType() == RigidBody::ColliderType::Mesh)
	{
		//TODO: Implement
	}
	return false;
}

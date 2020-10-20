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
	//TODO: Remove
	float testTotalMomentum = 0;
	UpdatePhysicsRegions_RemoveNodes(gameTime);
	for (int i = 0; i < rigidBodies.size(); i++)
	{
		testTotalMomentum += rigidBodies[i]->GetMomentumFloat();
		UpdatePhysicsRegions_AddNodes(rigidBodies[i], gameTime);
		rigidBodies[i]->FixedUpdate(gameTime);
	}
	CheckCollisions(1, gameTime);
	std::cout << testTotalMomentum << std::endl;
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
	rb->tempIndex = rigidBodies.size() - 1;
	//TODO: Remove
	if(rigidBodies.size() == 1)
		rigidBodies[0]->SetVelocity(glm::vec3(0.9, 0, 0));
	//if(rigidBodies.size() == 2)
	//	rigidBodies[1]->SetVelocity(glm::vec3(1, 0, 0));

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
void PhysicsManager::CheckCollisions(int iterations, float gameTime)
{
	for(int currentIteration = 0; currentIteration < iterations; currentIteration++)
	{
		//for (int i = 0; i < physicsRegions.size(); i++)
		for (std::vector<std::vector<PhysicsRegion>> physicsRegionI : physicsRegions)
		{
			for (std::vector<PhysicsRegion> physicsRegionIJ : physicsRegionI)
			{
				for (PhysicsRegion physicsRegion : physicsRegionIJ)
				{
					if (physicsRegion.startNode == nullptr)
						continue;
					RigidBodyNode* rbNode1 = physicsRegion.startNode;
					RigidBodyNode* rbNode2 = physicsRegion.startNode->nextNode;
					if (rbNode2 == nullptr)
						continue;
					while (rbNode1->nextNode != nullptr)
					{
						if (rbNode1 != rbNode2)
						{
							//Check the collision
							if(IsColliding(rbNode1, rbNode2, gameTime))
							{
								glm::vec3 rbPos1 = rbNode1->rigidBody->componentParent->GetTranslation() + rbNode1->rigidBody->GetPositionOffset();
								glm::vec3 rbPos2 = rbNode2->rigidBody->componentParent->GetTranslation() + rbNode2->rigidBody->GetPositionOffset();
								//glm::vec3 rbPosDiff = rbPos2 - rbPos1;
								//float rbDistance = sqrtf(powf(rbPosDiff.x, 2) + powf(rbPosDiff.y, 2) + powf(rbPosDiff.z, 2));

								std::cout << "RigidBody " << rbNode1->rigidBody->tempIndex << " has collided with " << rbNode2->rigidBody->tempIndex << "!" << std::endl;
								//std::cout << "Distance between rigid bodies is: " << glm::length(rbPos1 - rbPos2) << std::endl;

								glm::vec3 velocity1 = rbNode1->rigidBody->GetVelocity();
								glm::vec3 velocity2 = rbNode2->rigidBody->GetVelocity();
								float mass1 = rbNode1->rigidBody->GetMass();
								float mass2 = rbNode2->rigidBody->GetMass();

								glm::vec3 normalDirection = glm::normalize(rbPos1 - rbPos2);

								glm::vec3 velocityToStore1 = glm::vec3(0);// = (velocity1 * (mass1 - mass2) / (mass2 + mass1)) + ((2.f * mass2 * velocity2) / (mass2 + mass1));
								glm::vec3 velocityToStore2 = glm::vec3(0);// = ((velocity1 * 2.f * mass1) / (mass2 + mass1)) + (velocity2 * (mass2 - mass1) / (mass2 + mass1));
								
#pragma region DUD


								//velocityToStore1 = ((2 * mass2) / (mass1 + mass2)) *
								//	((velocity1 - velocity2, rbPos1 - rbPos2) / (abs(rbPos1 - rbPos2) * abs(rbPos1 - rbPos2))) *
								//(rbPos1 - rbPos2);
								//velocityToStore2 = ((2 * mass1) / (mass1 + mass2)) *
								//	((velocity2 - velocity1, rbPos2 - rbPos1) / (abs(rbPos2 - rbPos1) * abs(rbPos2 - rbPos1))) * 
								//	(rbPos2 - rbPos1);

								//velocityToStore1 = (rbPos1 - rbPos2) * glm::length(((mass1 - mass2)*velocity1 + (2 * mass2 * velocity2)) / (mass1 + mass2));
								//velocityToStore2 = (rbPos2 - rbPos1) * glm::length(((2 * mass1 * velocity1) - ((mass1 - mass2)*velocity2)) / (mass1 + mass2));

								//if (isnan(velocityToStore1.x))
								//	velocityToStore1.x = 0;
								//if (isnan(velocityToStore1.y))
								//	velocityToStore1.y = 0;
								//if (isnan(velocityToStore1.z))
								//	velocityToStore1.z = 0;
								////velocityToStore2 = velocity1 + velocity2 - velocityToStore1;
								//if (isnan(velocityToStore2.x))
								//	velocityToStore2.x = 0;
								//if (isnan(velocityToStore2.y))
								//	velocityToStore2.y = 0;
								//if (isnan(velocityToStore2.z))
								//	velocityToStore2.z = 0;
								//velocityToStore1 = glm::length(velocityToStore1) * glm::normalize(rbPos1 - rbPos2);
								//float v2FinalMagnitude = glm::length(velocityToStore1);
								//velocityToStore2 = v2FinalMagnitude * glm::normalize(rbPos2 - rbPos1);
								//velocityToStore1 = velocityToStore2 + velocity2 - velocity1;
#pragma endregion
								//velocityToStore1 = (rbNode2->rigidBody->GetMomentum() * mass2 / mass1) * (rbPos1 - rbPos2);
								//velocityToStore2 = (rbNode1->rigidBody->GetMomentum() * mass1 / mass2) * (rbPos2 - rbPos1);

								//velocityToStore1 = ((velocity1) + glm::normalize(rbPos1 - rbPos2) * glm::dot(glm::normalize(rbPos1 - rbPos2), glm::normalize(velocity1)) * (mass2 / mass1));
								//velocityToStore2 = ((velocity2) + glm::normalize(rbPos2 - rbPos1) * glm::dot(glm::normalize(rbPos2 - rbPos2), glm::normalize(velocity2)) * (mass1 / mass2));

								velocityToStore1 = velocity1 + (normalDirection * (mass2 / mass1));// * glm::length(velocity2);
								velocityToStore2 = velocity2 + (-normalDirection * (mass1 / mass2));// * glm::length(velocity2);

								if(velocityToStore1 != glm::vec3(0))
									velocityToStore1 *= (glm::dot(normalDirection, glm::normalize(velocityToStore1)));
								if(velocityToStore2 != glm::vec3(0))
									velocityToStore2 *= (glm::dot(-normalDirection, glm::normalize(velocityToStore2)));
								
								float momentumToStore1 = glm::length(velocityToStore1) * mass1;
								float momentumToStore2 = glm::length(velocityToStore2) * mass2;
								
								float momentum1 = rbNode1->rigidBody->GetMomentumFloat();
								float momentum2 = rbNode2->rigidBody->GetMomentumFloat();
								
								while (momentumToStore1 + momentumToStore2 > momentum1 + momentum2)
								{
									velocityToStore1 *= 0.99;
									velocityToStore2 *= 0.99;
								
									momentumToStore1 = glm::length(velocityToStore1) * mass1;
									momentumToStore2 = glm::length(velocityToStore2) * mass2;
								}
#pragma region DUD


								//velocityToStore1 = velocity1 +
								//	((2 * mass2) / (mass1 + mass2)) *
								//	((velocity1 - velocity2, rbPos1 - rbPos2) / (glm::abs(rbPos1 - rbPos2) * glm::abs(rbPos1 - rbPos2))) *
								//	(rbPos1 - rbPos2);
								//
								//velocityToStore2 = velocity2 -
								//	((2 * mass1) / (mass1 + mass2)) *
								//	((velocity2 - velocity1, rbPos2 - rbPos1) / (glm::abs(rbPos2 - rbPos1) * glm::abs(rbPos2 - rbPos1))) *
								//	(rbPos2 - rbPos1);

								//if (isnan(velocityToStore1.x))
								//	velocityToStore1.x = 0;
								//if (isnan(velocityToStore1.y))
								//	velocityToStore1.y = 0;
								//if (isnan(velocityToStore1.z))
								//	velocityToStore1.z = 0;
								//
								//if (isnan(velocityToStore2.x))
								//	velocityToStore2.x = 0;
								//if (isnan(velocityToStore2.y))
								//	velocityToStore2.y = 0;
								//if (isnan(velocityToStore2.z))
								//	velocityToStore2.z = 0;

								//glm::vec3 n = (rbPos1 - rbPos2) / (glm::abs(rbPos1) - glm::abs(rbPos2));
								//glm::vec3 vRelative = velocity1 - velocity2;
								//glm::vec3 vNormal = (vRelative*n)*n;
								//velocityToStore1 = velocity1 - vNormal;
								//velocityToStore2 = velocity2 + vNormal;


#pragma endregion								
								//
								//float totalMomentum = momentum1 + momentum2;
								//float totalMomentumToStore = momentumToStore1 + momentumToStore2;
								//
								//assert(totalMomentumToStore <= totalMomentum);

								//assert(abs((glm::length(velocity1) + glm::length(velocity2)) - (glm::length(velocityToStore1) + glm::length(velocityToStore2))) > 0.3f);
								//Store velocity in the storedVelocity variable this frame. storedVelocity is applied to velocity after calculating all collisions

								//velocityToStore1 = (velocity1 * (mass1 - mass2) + 2 * mass2 * velocity2) / (mass1 + mass2);
								//velocityToStore2 = (velocity2 * (mass2 - mass1) + 2 * mass1 * velocity1) / (mass1 + mass2);

								//mass1 = (2 * rbNode2->rigidBody->GetMass()) / (rbNode1->rigidBody->GetMass() + rbNode2->rigidBody->GetMass());
								//mass2 = (2 * rbNode1->rigidBody->GetMass()) / (rbNode1->rigidBody->GetMass() + rbNode2->rigidBody->GetMass());
								//velocityToStore1 = velocity1 - (mass1 * glm::dot(rbNode1->rigidBody->GetMomentumVec3() - rbNode2->rigidBody->GetMomentumVec3(), glm::normalize(rbPos1 - rbPos2)) / pow(glm::length(glm::normalize(rbPos1 - rbPos2)), 2)) * glm::normalize(rbPos1 - rbPos2);
								//velocityToStore2 = velocity2 - (mass2 * glm::dot(rbNode2->rigidBody->GetMomentumVec3() - rbNode1->rigidBody->GetMomentumVec3(), glm::normalize(rbPos2 - rbPos1)) / pow(glm::length(glm::normalize(rbPos2 - rbPos1)), 2)) * glm::normalize(rbPos2 - rbPos1);

								rbNode1->rigidBody->StoreVelocity(velocityToStore1);
								rbNode2->rigidBody->StoreVelocity(velocityToStore2);

								std::cout << "Previous total momentum = " << rbNode1->rigidBody->GetMomentumFloat() + rbNode2->rigidBody->GetMomentumFloat() << std::endl;
								std::cout << "New total momentum = " << (rbNode1->rigidBody->GetStoredMomentum()) + (rbNode2->rigidBody->GetStoredMomentum()) << std::endl;
								std::cout << "Change in total momentum: " << ((rbNode1->rigidBody->GetStoredMomentum() + rbNode2->rigidBody->GetStoredMomentum()) - (rbNode1->rigidBody->GetMomentumFloat() + rbNode2->rigidBody->GetMomentumFloat())) << std::endl << std::endl;

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
bool PhysicsManager::IsColliding(RigidBodyNode* rbNode1, RigidBodyNode* rbNode2, float gameTime)
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

	//Calculate collision for Axis-aligned bounding boxes (Cubes)
	RigidBody::RigidBodyProjections rbProjections1 = rbNode1->rigidBody->CalculateProjections(true, true, gameTime);
	RigidBody::RigidBodyProjections rbProjections2 = rbNode2->rigidBody->CalculateProjections(true, true, gameTime);
	if (rbProjections1.x[0] > rbProjections2.x[1] || rbProjections2.x[0] > rbProjections1.x[1])
		return false;
	if (rbProjections1.y[0] > rbProjections2.y[1] || rbProjections2.y[0] > rbProjections1.y[1])
		return false;
	if (rbProjections1.z[0] > rbProjections2.z[1] || rbProjections2.z[0] > rbProjections1.z[1])
		return false;

	if (rbNode1->rigidBody->GetColliderType() == RigidBody::ColliderType::Rectangle &&
		rbNode2->rigidBody->GetColliderType() == RigidBody::ColliderType::Rectangle)
	{
		return true;
	}
	else if (rbNode1->rigidBody->GetColliderType() == RigidBody::ColliderType::Sphere &&
		rbNode2->rigidBody->GetColliderType() == RigidBody::ColliderType::Sphere)
	{
		float distanceBetweenRBs = glm::length(rbNode1->rigidBody->GetPosition() - rbNode2->rigidBody->GetPosition());
		float radius1 = glm::length(rbNode1->rigidBody->GetScale());
		float radius2 = glm::length(rbNode2->rigidBody->GetScale());
		if (distanceBetweenRBs <= radius1 + radius2)
		{
			return true;
		}
	}

	return false;
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

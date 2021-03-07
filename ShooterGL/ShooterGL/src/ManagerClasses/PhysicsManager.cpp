#include "PhysicsManager.h"

PhysicsManager* PhysicsManager::instance = 0;

PhysicsManager::PhysicsManager()
{
}


PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::Initialize(glm::vec3 regionBounds, glm::vec3 regionCount)
{
	rigidBodies.clear();
#pragma region Delete rigid body nodes from each physics region
	for (int i = 0; i < physicsRegions.size(); i++)
	{
		for (int j = 0; j < physicsRegions[i].size(); j++)
		{
			for (int k = 0; k < physicsRegions[i][j].size(); k++)
			{
				if (physicsRegions[i][j][k].startNode == nullptr)
					continue;
				RigidBodyNode* currentNode = physicsRegions[i][j][k].startNode;
				RigidBodyNode* nextNode = currentNode->nextNode;
				
				while (currentNode != nullptr)
				{
					//Delete the start node of the physics regions, which then deletes all subsequent rigidbody nodes
					delete(currentNode);
					currentNode = nextNode;
					if(currentNode != nullptr)
						nextNode = currentNode->nextNode;
				}
				physicsRegions[i][j][k].startNode = nullptr;
			}
		}
	}
	physicsRegions.clear();
#pragma endregion

	totalPhysicsRegions = regionCount;
	//Set up each physics region with it's own bounds
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
	outerPhysicsRegion.startBound = physicsRegions[0][0][0].startBound;
	outerPhysicsRegion.endBound = physicsRegions[regionCount.x - 1][regionCount.y - 1][regionCount.z - 1].endBound;
}

void PhysicsManager::FixedUpdate(float gameTime)
{
	//TODO: Remove
	//float testTotalMomentum = 0;
	UpdatePhysicsRegions_RemoveNodes(gameTime);
	for (int i = 0; i < rigidBodies.size(); i++)
	{
		//testTotalMomentum += rigidBodies[i]->GetMomentumFloat();
		UpdatePhysicsRegions_AddNodes(rigidBodies[i], gameTime);
		rigidBodies[i]->FixedUpdate(gameTime);
	}
	CheckCollisions(1, gameTime);
	//std::cout << testTotalMomentum << std::endl;
}

//Loop through all regions. Add nodes to a region if they entered the region this frame
void PhysicsManager::UpdatePhysicsRegions_AddNodes(RigidBody * rb, float gameTime)
{
	for (int i = 0; i < rb->GetColliders().size(); i++)
	{
		Collider* currentCollider = rb->GetColliderRef(i);
		glm::vec3 physicsRegionToAdd = (rb->componentParent->GetTranslation() + currentCollider->positionOffset) / physicsRegionBounds;
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
						if (!RigidBodyInRegion(rb, physicsRegions[physicsRegionRangeToCheck.x][physicsRegionRangeToCheck.y][physicsRegionRangeToCheck.z], false, gameTime) && RigidBodyInRegion(rb, physicsRegions[physicsRegionRangeToCheck.x][physicsRegionRangeToCheck.y][physicsRegionRangeToCheck.z], true, gameTime))
						{
							//add it to the region
							AddRigidBodyToRegion(rb, physicsRegionRangeToCheck);
							std::cout << "Added rigidBody: " << rb << " to physics region " << physicsRegionRangeToCheck.x << ", " << physicsRegionRangeToCheck.y << ", " << physicsRegionRangeToCheck.z << std::endl;
						}
					}
				}
			}
		}
		//Determine whether or not to add a rigid body to the outer physics region
		//TODO: Rename outerPhysicsRegion?
		//If rigidBody just entered the outer physics region
		if(!RigidBodyInRegion(rb, outerPhysicsRegion, true, gameTime) && RigidBodyInRegion(rb, outerPhysicsRegion, false, gameTime))
		{
			RigidBodyNode* oldStartNode = outerPhysicsRegion.startNode;
			outerPhysicsRegion.startNode = new RigidBodyNode();
			outerPhysicsRegion.startNode->rigidBody = rb;
			outerPhysicsRegion.startNode->nextNode = oldStartNode;
			std::cout << "Added rigidBody: " << rb << " to the outer physics region." << std::endl;
		}
	}
}

//When adding a rigidbody to a scene (during loading or instantiation)
void PhysicsManager::InitializeRigidBody(RigidBody * rb, float gameTime)
{
	rigidBodies.push_back(rb);

	glm::vec3 physicsRegionToAdd = (rb->componentParent->GetTranslation()/* + rb->GetPositionOffset()*/) / physicsRegionBounds;
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
				if (RigidBodyInRegion(rb, physicsRegions[physicsRegionRangeToCheck.x][physicsRegionRangeToCheck.y][physicsRegionRangeToCheck.z], false, gameTime))
				{
					//add it to the region
					AddRigidBodyToRegion(rb, physicsRegionRangeToCheck);
					std::cout << "Added rigidBody: " << rb << " to physics region " << physicsRegionRangeToCheck.x << ", " << physicsRegionRangeToCheck.y << ", " << physicsRegionRangeToCheck.z << std::endl;
				}
			}
		}
	}
	//If rigidBody spawned inside the outer physics region
	if (!RigidBodyInRegion(rb, outerPhysicsRegion, false, gameTime))
	{
		RigidBodyNode* oldStartNode = outerPhysicsRegion.startNode;
		outerPhysicsRegion.startNode = new RigidBodyNode();
		outerPhysicsRegion.startNode->rigidBody = rb;
		outerPhysicsRegion.startNode->nextNode = oldStartNode;
		std::cout << "Added rigidBody: " << rb << " to the outer physics region." << std::endl;
	}

 }

//Replace the start node of a region with a new one containing the RigidBody* argument
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
	RigidBodyNode* prevRBNode = nullptr;
	RigidBodyNode* rbNode = outerPhysicsRegion.startNode;
	
	//Remove rigidbodies from outer physics region
	while (rbNode != nullptr)
	{
		//If the rigidBody isn't inside the region
		if (RigidBodyInRegion(rbNode->rigidBody, outerPhysicsRegion, false, gameTime) && RigidBodyInRegion(rbNode->rigidBody, outerPhysicsRegion, true, gameTime))
		{
			std::cout << "Removed rigidBody: " << rbNode->rigidBody << " from outer physics region." << std::endl;
			//If not at the head node for the physics region
			if (prevRBNode != nullptr && rbNode != outerPhysicsRegion.startNode)
			{
				//Remove current Node
				prevRBNode->nextNode = rbNode->nextNode;
				delete(rbNode);
				rbNode = prevRBNode->nextNode;
			}
			else
				//If at the head node for the physics region
			{
				outerPhysicsRegion.startNode = rbNode->nextNode;
				delete(rbNode);
				rbNode = outerPhysicsRegion.startNode;
			}
		}
		else
		{
			prevRBNode = rbNode;
			rbNode = rbNode->nextNode;
		}
	}


	//Remove rigidbodies from all indexed physics regions
	glm::vec3 regionIDs;
	prevRBNode = nullptr;
	for (regionIDs.x = 0; regionIDs.x < physicsRegions.size(); regionIDs.x++)
	{
		for (regionIDs.y = 0; regionIDs.y < physicsRegions[regionIDs.x].size(); regionIDs.y++)
		{
			for (regionIDs.z = 0; regionIDs.z < physicsRegions[regionIDs.x][regionIDs.y].size(); regionIDs.z++)
			{
				rbNode = physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startNode;
				while (rbNode != nullptr)
				{
					//If the rigidBody isn't inside the region
					if (!RigidBodyInRegion(rbNode->rigidBody, physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z], false, gameTime) && !RigidBodyInRegion(rbNode->rigidBody, physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z], true, gameTime))
					{
						std::cout << "Removed rigidBody: " << rbNode->rigidBody << " from physics region " << regionIDs.x << ", " << regionIDs.y << ", " << regionIDs.z << std::endl;
						//If not at the head node for the physics region
						if (prevRBNode != nullptr && rbNode != physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z].startNode)
						{
							//Remove current Node
							prevRBNode->nextNode = rbNode->nextNode;
							delete(rbNode);
							rbNode = prevRBNode->nextNode;
						}
						else
							//If at the head node for the physics region
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
#pragma region Check collisions for rigidbodies outside indexed physics regions
		//Check collisions for all rigidbodies outside indexed physics regions
		RigidBodyNode* rbNode1 = outerPhysicsRegion.startNode;
		RigidBodyNode* rbNode2 = outerPhysicsRegion.startNode->nextNode;
		if (rbNode2 == nullptr)
			continue;
		while (rbNode1->nextNode != nullptr)
		{
			if ((rbNode1->rigidBody->GetIsActive() && rbNode2->rigidBody->GetIsActive()) && rbNode1 != rbNode2)
			{
				//Check the collision
				for (int i = 0; i < rbNode1->rigidBody->GetColliders().size(); i++)
				{
					Collider* currentCollider1 = rbNode1->rigidBody->GetColliderRef(i);
					for (int j = 0; j < rbNode2->rigidBody->GetColliders().size(); j++)
					{
						//Players don't bounce off of each other //TODO: Check for other solutions
						if (rbNode1->rigidBody->componentParent->FindController() && rbNode2->rigidBody->componentParent->FindController())
							continue;
						//if one of the rigidbodies were spawned by the other colliding entity
						else if (rbNode1->rigidBody->componentParent == rbNode2->rigidBody->GetSpawnedBy() ||
							rbNode2->rigidBody->componentParent == rbNode1->rigidBody->GetSpawnedBy())
							continue;

						Collider* currentCollider2 = rbNode2->rigidBody->GetColliderRef(j);

						if (IsColliding(currentCollider1, currentCollider2, gameTime))
						{
							RigidBody* rb1 = rbNode1->rigidBody;
							RigidBody* rb2 = rbNode2->rigidBody;
							if (currentCollider1->isTrigger)
								rb2->componentParent->OnTriggerEnter(rb1->componentParent);
							else
								rb2->componentParent->OnCollisionEnter(rb1->componentParent);
							if (currentCollider2->isTrigger)
								rb1->componentParent->OnTriggerEnter(rb2->componentParent);
							else
								rb1->componentParent->OnCollisionEnter(rb2->componentParent);
							if (currentCollider1->isTrigger || currentCollider2->isTrigger)
								continue;

							glm::vec3 colliderPos1 = rb1->componentParent->GetTranslation() + currentCollider1->positionOffset;
							glm::vec3 colliderPos2 = rb2->componentParent->GetTranslation() + currentCollider2->positionOffset;

							glm::vec3 velocity1 = rbNode1->rigidBody->GetVelocity();
							glm::vec3 velocity2 = rbNode2->rigidBody->GetVelocity();
							float mass1 = rbNode1->rigidBody->GetMass();
							float mass2 = rbNode2->rigidBody->GetMass();

							glm::vec3 normalDirection = glm::normalize(colliderPos1 - colliderPos2);

							glm::vec3 velocityToStore1 = glm::vec3(0);// = (velocity1 * (mass1 - mass2) / (mass2 + mass1)) + ((2.f * mass2 * velocity2) / (mass2 + mass1));
							glm::vec3 velocityToStore2 = glm::vec3(0);// = ((velocity1 * 2.f * mass1) / (mass2 + mass1)) + (velocity2 * (mass2 - mass1) / (mass2 + mass1));

							velocityToStore1 = (colliderPos1 - colliderPos2) * glm::length(((mass1 - mass2)*velocity1 + (2 * mass2 * velocity2)) / (mass1 + mass2));
							velocityToStore2 = (colliderPos2 - colliderPos1) * glm::length(((2 * mass1 * velocity1) - ((mass1 - mass2)*velocity2)) / (mass1 + mass2));

							if (isnan(velocityToStore1.x))
								velocityToStore1.x = 0;
							if (isnan(velocityToStore1.y))
								velocityToStore1.y = 0;
							if (isnan(velocityToStore1.z))
								velocityToStore1.z = 0;
							if (isnan(velocityToStore2.x))
								velocityToStore2.x = 0;
							if (isnan(velocityToStore2.y))
								velocityToStore2.y = 0;
							if (isnan(velocityToStore2.z))
								velocityToStore2.z = 0;

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

							rbNode1->rigidBody->StoreVelocity(velocityToStore1);
							rbNode2->rigidBody->StoreVelocity(velocityToStore2);
						}
					}
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
#pragma endregion
#pragma region Check collisions fora ll rigidbodies inside indexed physics regions
		//Check collisions for all rigidbodies inside indexed physics regions
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
						if ((rbNode1->rigidBody->GetIsActive() && rbNode2->rigidBody->GetIsActive()) && rbNode1 != rbNode2)
						{
							//Check the collision
							for (int i = 0; i < rbNode1->rigidBody->GetColliders().size(); i++)
							{
								Collider* currentCollider1 = rbNode1->rigidBody->GetColliderRef(i);
								for (int j = 0; j < rbNode2->rigidBody->GetColliders().size(); j++)
								{
									//Players don't bounce off of each other //TODO: Check for other solutions
									if (rbNode1->rigidBody->componentParent->FindController() && rbNode2->rigidBody->componentParent->FindController())
										continue;
									//if one of the rigidbodies were spawned by the other colliding entity
									else if (rbNode1->rigidBody->componentParent == rbNode2->rigidBody->GetSpawnedBy() ||
										rbNode2->rigidBody->componentParent == rbNode1->rigidBody->GetSpawnedBy())
										continue;

									Collider* currentCollider2 = rbNode2->rigidBody->GetColliderRef(j);

									if (IsColliding(currentCollider1, currentCollider2, gameTime))
									{
										RigidBody* rb1 = rbNode1->rigidBody;
										RigidBody* rb2 = rbNode2->rigidBody;
										if (currentCollider1->isTrigger)
											rb2->componentParent->OnTriggerEnter(rb1->componentParent);
										else
											rb2->componentParent->OnCollisionEnter(rb1->componentParent);
										if (currentCollider2->isTrigger)
											rb1->componentParent->OnTriggerEnter(rb2->componentParent);
										else
											rb1->componentParent->OnCollisionEnter(rb2->componentParent);
										if (currentCollider1->isTrigger || currentCollider2->isTrigger)
											continue;

										glm::vec3 colliderPos1 = rb1->componentParent->GetTranslation() + currentCollider1->positionOffset;
										glm::vec3 colliderPos2 = rb2->componentParent->GetTranslation() + currentCollider2->positionOffset;

										glm::vec3 velocity1 = rbNode1->rigidBody->GetVelocity();
										glm::vec3 velocity2 = rbNode2->rigidBody->GetVelocity();
										float mass1 = rbNode1->rigidBody->GetMass();
										float mass2 = rbNode2->rigidBody->GetMass();

										glm::vec3 normalDirection = glm::normalize(colliderPos1 - colliderPos2);

										glm::vec3 velocityToStore1 = glm::vec3(0);// = (velocity1 * (mass1 - mass2) / (mass2 + mass1)) + ((2.f * mass2 * velocity2) / (mass2 + mass1));
										glm::vec3 velocityToStore2 = glm::vec3(0);// = ((velocity1 * 2.f * mass1) / (mass2 + mass1)) + (velocity2 * (mass2 - mass1) / (mass2 + mass1));

										velocityToStore1 = (colliderPos1 - colliderPos2) * glm::length(((mass1 - mass2)*velocity1 + (2 * mass2 * velocity2)) / (mass1 + mass2));
										velocityToStore2 = (colliderPos2 - colliderPos1) * glm::length(((2 * mass1 * velocity1) - ((mass1 - mass2)*velocity2)) / (mass1 + mass2));

										if (isnan(velocityToStore1.x))
											velocityToStore1.x = 0;
										if (isnan(velocityToStore1.y))
											velocityToStore1.y = 0;
										if (isnan(velocityToStore1.z))
											velocityToStore1.z = 0;
										if (isnan(velocityToStore2.x))
											velocityToStore2.x = 0;
										if (isnan(velocityToStore2.y))
											velocityToStore2.y = 0;
										if (isnan(velocityToStore2.z))
											velocityToStore2.z = 0;

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

										rbNode1->rigidBody->StoreVelocity(velocityToStore1);
										rbNode2->rigidBody->StoreVelocity(velocityToStore2);
									}
								}
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
#pragma endregion
}

//Check for collisions using vector projections on the x, y, and z axes.
bool PhysicsManager::IsColliding(Collider * collider1, Collider * collider2, float gameTime)
{
	//Calculate collision for Axis-aligned bounding boxes (Cubes)
	Collider::ColliderProjections rbProjections1 = collider1->CalculateProjections(true, true, gameTime);
	Collider::ColliderProjections rbProjections2 = collider2->CalculateProjections(true, true, gameTime);
	if (rbProjections1.x[0] > rbProjections2.x[1] || rbProjections2.x[0] > rbProjections1.x[1])
		return false;
	if (rbProjections1.y[0] > rbProjections2.y[1] || rbProjections2.y[0] > rbProjections1.y[1])
		return false;
	if (rbProjections1.z[0] > rbProjections2.z[1] || rbProjections2.z[0] > rbProjections1.z[1])
		return false;

	if (collider1->colliderType == Collider::ColliderType::Rectangle &&
		collider2->colliderType == Collider::ColliderType::Rectangle)
	{
		return true;
	}
	else if (collider1->colliderType == Collider::ColliderType::Sphere &&
		collider2->colliderType == Collider::ColliderType::Sphere)
	{
		float distanceBetweenRBs = glm::length((collider1->colliderParent->GetPosition() + collider1->positionOffset) -
			(collider2->colliderParent->GetPosition() + collider2->positionOffset));
		float radius1 = glm::length(collider1->scale);
		float radius2 = glm::length(collider2->scale);
		if (distanceBetweenRBs <= radius1 + radius2)
		{
			return true;
		}
	}
	return false;
}

//Checks if a rigidbody is inside a region
bool PhysicsManager::RigidBodyInRegion(RigidBody * rb, PhysicsRegion& currentPhysicsRegion, bool addRB_Velocity, float gameTime)
{
	for (int i = 0; i < rb->GetColliders().size(); i++)
	{
		Collider* currentCollider = rb->GetColliderRef(i);
		//rectangle collider
		if (currentCollider->colliderType == Collider::ColliderType::Rectangle)
		{
			float rb_xPlusEdge = rb->componentParent->GetTranslation().x + (currentCollider->positionOffset.x + currentCollider->scale.x);
			float rb_xMinusEdge = rb->componentParent->GetTranslation().x + (currentCollider->positionOffset.x - currentCollider->scale.x);
			float rb_yPlusEdge = rb->componentParent->GetTranslation().y + (currentCollider->positionOffset.y + currentCollider->scale.y);
			float rb_yMinusEdge = rb->componentParent->GetTranslation().y + (currentCollider->positionOffset.y - currentCollider->scale.y);
			float rb_zPlusEdge = rb->componentParent->GetTranslation().z + (currentCollider->positionOffset.z + currentCollider->scale.z);
			float rb_zMinusEdge = rb->componentParent->GetTranslation().z - (currentCollider->positionOffset.z + currentCollider->scale.z);

			if (addRB_Velocity)
			{
				const glm::vec3 rbVelocityConstant = rb->GetVelocity() * gameTime;
				rb_xPlusEdge += rbVelocityConstant.x;
				rb_xMinusEdge += rbVelocityConstant.x;
				rb_yPlusEdge += rbVelocityConstant.y;
				rb_yMinusEdge += rbVelocityConstant.y;
				rb_zPlusEdge += rbVelocityConstant.z;
				rb_zMinusEdge += rbVelocityConstant.z;
			}

			//if one of the x-axis aligned edges of the cube are outside the physics region
			if (!(rb_xPlusEdge >= currentPhysicsRegion.startBound.x &&
				rb_xPlusEdge <= currentPhysicsRegion.endBound.x) &&
				!(rb_xMinusEdge >= currentPhysicsRegion.startBound.x &&
					rb_xMinusEdge <= currentPhysicsRegion.endBound.x))
			{
				return false;
			}
			//if one of the y-axis aligned edges of the cube are outside the physics region
			if (!(rb_yPlusEdge >= currentPhysicsRegion.startBound.y &&
				rb_yPlusEdge <= currentPhysicsRegion.endBound.y) &&
				!(rb_yMinusEdge >= currentPhysicsRegion.startBound.y &&
					rb_yMinusEdge <= currentPhysicsRegion.endBound.y))
			{
				return false;
			}
			//if one of the z-axis aligned edges of the cube are outside the physics region
			if (!(rb_zPlusEdge >= currentPhysicsRegion.startBound.z &&
				rb_zPlusEdge <= currentPhysicsRegion.endBound.z) &&
				!(rb_zMinusEdge >= currentPhysicsRegion.startBound.z &&
					rb_zMinusEdge <= currentPhysicsRegion.endBound.z))
			{
				return false;
			}
			return true;
		}

		//sphere collider
		else if (currentCollider->colliderType == Collider::ColliderType::Sphere)
		{
			glm::vec3 rigidBodyCenter = rb->componentParent->GetTranslation() + currentCollider->positionOffset;
			glm::vec3 regionCenter = (currentPhysicsRegion.startBound + currentPhysicsRegion.endBound) / glm::vec3(2);
			glm::vec3 closestSpherePoint = rigidBodyCenter + (normalize(regionCenter - rigidBodyCenter) * currentCollider->scale);

			if (addRB_Velocity)
			{
				const glm::vec3 rbVelocityConstant = rb->GetVelocity() * gameTime;
				closestSpherePoint += rbVelocityConstant;
			}

			PhysicsRegion& region = currentPhysicsRegion;

			if (closestSpherePoint.x >= region.startBound.x &&
				closestSpherePoint.x <= region.endBound.x &&
				closestSpherePoint.y >= region.startBound.y &&
				closestSpherePoint.y <= region.endBound.y &&
				closestSpherePoint.z >= region.startBound.z &&
				closestSpherePoint.z <= region.endBound.z)
				return true;
		}

		//mesh collider
		//TODO: Implement mesh collider (GJK algorithm?) (Not needed for minimum viable product)
		else if (currentCollider->colliderType == Collider::ColliderType::Mesh)
		{
			//TODO: Implement
			if (addRB_Velocity)
			{
				const glm::vec3 rbVelocityConstant = rb->GetVelocity() * gameTime;
			}
		}
		return false;
	}
}

PhysicsManager* PhysicsManager::GetInstance()
{
	if (!instance)
		instance = new PhysicsManager();
	return instance;
}

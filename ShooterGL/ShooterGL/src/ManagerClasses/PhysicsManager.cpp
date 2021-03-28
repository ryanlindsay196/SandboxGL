#include "PhysicsManager.h"
#include <math.h>

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
#pragma region Delete rigid body nodes from the outer physics region
	if (outerPhysicsRegion.startNode != nullptr)
	{
		RigidBodyNode* currentNode = outerPhysicsRegion.startNode;
		RigidBodyNode* nextNode = currentNode->nextNode;

		while (currentNode != nullptr)
		{
			//Delete the start node of the physics regions, which then deletes all subsequent rigidbody nodes
			delete(currentNode);
			currentNode = nextNode;
			if (currentNode != nullptr)
				nextNode = currentNode->nextNode;
		}
		outerPhysicsRegion.startNode = nullptr;
	}
#pragma endregion
#pragma region Delete rigid body nodes from each indexed physics region
	for (unsigned int i = 0; i < physicsRegions.size(); i++)
	{
		for (unsigned int j = 0; j < physicsRegions[i].size(); j++)
		{
			for (unsigned int k = 0; k < physicsRegions[i][j].size(); k++)
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
#pragma region Set up each indexed physics region with its own bounds
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
#pragma endregion
}

void PhysicsManager::FixedUpdate(float gameTime)
{
	UpdatePhysicsRegions_RemoveNodes(gameTime);
	for (unsigned int i = 0; i < rigidBodies.size(); i++)
	{
		UpdatePhysicsRegions_AddNodes(&rigidBodies[i], gameTime, i);
		rigidBodies[i].FixedUpdate(gameTime);
	}
	CheckCollisions(1, gameTime);
}

//Loop through all regions. Add nodes to a region if they entered the region this frame
void PhysicsManager::UpdatePhysicsRegions_AddNodes(RigidBody * rb, float gameTime, unsigned int rigidBodyIndex)
{
	for (unsigned int i = 0; i < rb->GetColliders().size(); i++)
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
						if (!RigidBodyInRegion(rigidBodyIndex, physicsRegions[physicsRegionRangeToCheck.x][physicsRegionRangeToCheck.y][physicsRegionRangeToCheck.z], false, gameTime) && RigidBodyInRegion(rigidBodyIndex, physicsRegions[physicsRegionRangeToCheck.x][physicsRegionRangeToCheck.y][physicsRegionRangeToCheck.z], true, gameTime))
						{
							//add it to the region
							AddRigidBodyToRegion(rigidBodyIndex, physicsRegionRangeToCheck);
							std::cout << "Added rigidBody: " << rigidBodyIndex << " to physics region " << physicsRegionRangeToCheck.x << ", " << physicsRegionRangeToCheck.y << ", " << physicsRegionRangeToCheck.z << std::endl;
						}
					}
				}
			}
		}
		//Determine whether or not to add a rigid body to the outer physics region
		//If rigidBody just entered the outer physics region
		if(!RigidBodyInRegion(rigidBodyIndex, outerPhysicsRegion, true, gameTime) && RigidBodyInRegion(rigidBodyIndex, outerPhysicsRegion, false, gameTime))
		{
			RigidBodyNode* oldStartNode = outerPhysicsRegion.startNode;
			outerPhysicsRegion.startNode = new RigidBodyNode();
			outerPhysicsRegion.startNode->rigidBodyIndex = rigidBodyIndex;
			outerPhysicsRegion.startNode->nextNode = oldStartNode;
			std::cout << "Added rigidBody: " << rigidBodyIndex << " to the outer physics region." << std::endl;
		}
	}
}

//When adding a rigidbody to a scene (during loading or instantiation)
RigidBody* PhysicsManager::InitializeRigidBody(Entity* entity, float gameTime, std::vector<std::string> rigidBodyProperties, unsigned int newEntityComponentIndex)
{
	BaseManager::AddComponent<RigidBody>(rigidBodies, newEntityComponentIndex, entity);
	RigidBody& rb = (rigidBodies[rigidBodies.size() - 1]);
	rb.Initialize(rigidBodyProperties, nullptr);

	//if (rigidBodies.size() == 4)
	//	rb->SetVelocity(glm::vec3(1, 0, 0));
	glm::vec3 physicsRegionToAdd = (entity->GetTranslation()/* + rb->GetPositionOffset()*/) / physicsRegionBounds;
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
				if (RigidBodyInRegion(rigidBodies.size() - 1, physicsRegions[physicsRegionRangeToCheck.x][physicsRegionRangeToCheck.y][physicsRegionRangeToCheck.z], false, gameTime))
				{
					//add it to the region
					AddRigidBodyToRegion(rigidBodies.size() - 1, physicsRegionRangeToCheck);
					std::cout << "Added rigidBody: " << rigidBodies.size() - 1 << " to physics region " << physicsRegionRangeToCheck.x << ", " << physicsRegionRangeToCheck.y << ", " << physicsRegionRangeToCheck.z << std::endl;
				}
			}
		}
	}
	//If rigidBody spawned inside the outer physics region
	if (!RigidBodyInRegion(rigidBodies.size() - 1, outerPhysicsRegion, false, gameTime))
	{
		RigidBodyNode* oldStartNode = outerPhysicsRegion.startNode;
		outerPhysicsRegion.startNode = new RigidBodyNode();
		outerPhysicsRegion.startNode->rigidBodyIndex = rigidBodies.size() - 1;
		outerPhysicsRegion.startNode->nextNode = oldStartNode;
		std::cout << "Added rigidBody: " << rigidBodies.size() - 1 << " to the outer physics region." << std::endl;
	}
	return &rb;
 }

//Replace the start node of a region with a new one containing the RigidBody* argument
void PhysicsManager::AddRigidBodyToRegion(unsigned int rbIndex, glm::vec3 region)
{
	RigidBodyNode* oldStartNode = physicsRegions[region.x][region.y][region.z].startNode;
	physicsRegions[region.x][region.y][region.z].startNode = new RigidBodyNode();
	physicsRegions[region.x][region.y][region.z].startNode->rigidBodyIndex = rbIndex;
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
		if (RigidBodyInRegion(rbNode->rigidBodyIndex, outerPhysicsRegion, false, gameTime) && RigidBodyInRegion(rbNode->rigidBodyIndex, outerPhysicsRegion, true, gameTime))
		{
			std::cout << "Removed rigidBody: " << rbNode->rigidBodyIndex << " from outer physics region." << std::endl;
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
					if (!RigidBodyInRegion(rbNode->rigidBodyIndex, physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z], false, gameTime) && !RigidBodyInRegion(rbNode->rigidBodyIndex, physicsRegions[regionIDs.x][regionIDs.y][regionIDs.z], true, gameTime))
					{
						std::cout << "Removed rigidBody: " << rbNode->rigidBodyIndex << " from physics region " << regionIDs.x << ", " << regionIDs.y << ", " << regionIDs.z << std::endl;
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
		//Check collisions for all rigidbodies outside indexed physics regions
		CheckCollisionsInRegion(outerPhysicsRegion, gameTime);
		//Check collisions for all rigidbodies inside indexed physics regions
		for (std::vector<std::vector<PhysicsRegion>> physicsRegionI : physicsRegions)
		{
			for (std::vector<PhysicsRegion> physicsRegionIJ : physicsRegionI)
			{
				for (PhysicsRegion physicsRegion : physicsRegionIJ)
				{
					CheckCollisionsInRegion(physicsRegion, gameTime);
				}
			}
		}
	}
}

void PhysicsManager::CheckCollisionsInRegion(PhysicsRegion physicsRegion, float gameTime)
{

	if (physicsRegion.startNode == nullptr)
		return;
	RigidBodyNode* rbNode1 = physicsRegion.startNode;
	RigidBodyNode* rbNode2 = physicsRegion.startNode->nextNode;
	if (rbNode2 == nullptr)
		return;
	while (rbNode1->nextNode != nullptr)
	{
		if ((rigidBodies[rbNode1->rigidBodyIndex].GetIsActive() && rigidBodies[rbNode2->rigidBodyIndex].GetIsActive()) && rbNode1 != rbNode2)
		{
			//Check the collision
			for (unsigned int i = 0; i < rigidBodies[rbNode1->rigidBodyIndex].GetColliders().size(); i++)
			{
				Collider* currentCollider1 = rigidBodies[rbNode1->rigidBodyIndex].GetColliderRef(i);
				for (unsigned int j = 0; j < rigidBodies[rbNode2->rigidBodyIndex].GetColliders().size(); j++)
				{
					//Players don't bounce off of each other //TODO: Check for other solutions
					if (rigidBodies[rbNode1->rigidBodyIndex].componentParent->FindController() && rigidBodies[rbNode2->rigidBodyIndex].componentParent->FindController())
						continue;
					//if one of the rigidbodies were spawned by the other colliding entity
					else if (rigidBodies[rbNode1->rigidBodyIndex].componentParent == rigidBodies[rbNode2->rigidBodyIndex].GetSpawnedBy() ||
						rigidBodies[rbNode2->rigidBodyIndex].componentParent == rigidBodies[rbNode1->rigidBodyIndex].GetSpawnedBy())
						continue;

					Collider* currentCollider2 = rigidBodies[rbNode2->rigidBodyIndex].GetColliderRef(j);

					glm::vec3 normalDirection;

					if (IsColliding(currentCollider1, currentCollider2, gameTime, normalDirection))
					{
						RigidBody* rb1 = &rigidBodies[rbNode1->rigidBodyIndex];
						RigidBody* rb2 = &rigidBodies[rbNode2->rigidBodyIndex];
						
						rb1->OnCollisionEnter(nullptr);
						rb2->OnCollisionEnter(nullptr);
						continue; 

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

						glm::vec3 velocity1 = rigidBodies[rbNode1->rigidBodyIndex].GetVelocity();
						glm::vec3 velocity2 = rigidBodies[rbNode2->rigidBodyIndex].GetVelocity();
						float mass1 = rigidBodies[rbNode1->rigidBodyIndex].GetMass();
						float mass2 = rigidBodies[rbNode2->rigidBodyIndex].GetMass();

						glm::vec3 mass1Vec = glm::vec3(mass1); 
						glm::vec3 mass2Vec = glm::vec3(mass2);
						
						if (rb1->GetPositionConstraints().x == 0) { mass1Vec.x = 200000.f; }
						if (rb1->GetPositionConstraints().y == 0) { mass1Vec.y = 200000.f; }
						if (rb1->GetPositionConstraints().z == 0) { mass1Vec.z = 200000.f; }
						if (rb2->GetPositionConstraints().x == 0) { mass2Vec.x = 200000.f; }
						if (rb2->GetPositionConstraints().y == 0) { mass2Vec.y = 200000.f; }
						if (rb2->GetPositionConstraints().z == 0) { mass2Vec.z = 200000.f; }

						//glm::vec3 normalDirection = colliderPos1 - colliderPos2;

						glm::vec3 velocityToStore1 = glm::vec3(0);// = (velocity1 * (mass1 - mass2) / (mass2 + mass1)) + ((2.f * mass2 * velocity2) / (mass2 + mass1));
						glm::vec3 velocityToStore2 = glm::vec3(0);// = ((velocity1 * 2.f * mass1) / (mass2 + mass1)) + (velocity2 * (mass2 - mass1) / (mass2 + mass1));

						velocityToStore1 = (normalDirection) * glm::length(((mass1Vec - mass2Vec)*velocity1 + (2.f * mass2Vec * velocity2)) / (mass1Vec + mass2Vec));
						velocityToStore2 = (-normalDirection) * glm::length(((2.f * mass1Vec * velocity1) - ((mass1Vec - mass2Vec)*velocity2)) / (mass1Vec + mass2Vec));

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

						float momentum1 = rigidBodies[rbNode1->rigidBodyIndex].GetMomentumFloat();
						float momentum2 = rigidBodies[rbNode2->rigidBodyIndex].GetMomentumFloat();

						while (momentumToStore1 + momentumToStore2 > momentum1 + momentum2)
						{
							velocityToStore1 *= 0.99;
							velocityToStore2 *= 0.99;

							momentumToStore1 = glm::length(velocityToStore1) * mass1;
							momentumToStore2 = glm::length(velocityToStore2) * mass2;
						}

						//velocityToStore2 *= rbNode1->rigidBody->GetBounce();
						//velocityToStore1 *= rbNode2->rigidBody->GetBounce();

						rigidBodies[rbNode1->rigidBodyIndex].StoreVelocity(velocityToStore1);
						rigidBodies[rbNode2->rigidBodyIndex].StoreVelocity(velocityToStore2);
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

//Check for collisions using vector projections on the x, y, and z axes.
bool PhysicsManager::IsColliding(Collider * collider1, Collider * collider2, float gameTime, glm::vec3& normalDirection)
{
	glm::vec3 colliderPos1 = collider1->colliderParent->componentParent->GetTranslation() + collider1->positionOffset;
	glm::vec3 colliderPos2 = collider2->colliderParent->componentParent->GetTranslation() + collider2->positionOffset;

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
		float closestX = std::min(abs(rbProjections1.x[0] - rbProjections2.x[1]), abs(rbProjections2.x[1] - rbProjections1.x[0]));
		float closestY = std::min(abs(rbProjections1.y[0] - rbProjections2.y[1]), abs(rbProjections2.y[1] - rbProjections1.y[0]));
		float closestZ = std::min(abs(rbProjections1.z[0] - rbProjections2.z[1]), abs(rbProjections2.z[1] - rbProjections1.z[0]));
		if (closestX < closestY && closestX < closestZ)
			normalDirection = glm::vec3(1, 0, 0) * (colliderPos2.x - colliderPos1.x);
		else if (closestY < closestX && closestY < closestZ)
			normalDirection = glm::vec3(0, 1, 0) *(colliderPos2.y - colliderPos1.y);
		else
			normalDirection = glm::vec3(0, 0, 1) * (colliderPos2.z - colliderPos1.z);
		//normalDirection = (normalDirection);
		return true;
	}
	else if (collider1->colliderType == Collider::ColliderType::Sphere &&
		collider2->colliderType == Collider::ColliderType::Sphere)
	{
		normalDirection = -(colliderPos2 - colliderPos1);


		float distanceBetweenRBs = glm::length((collider1->colliderParent->GetPosition() + collider1->positionOffset) -
			(collider2->colliderParent->GetPosition() + collider2->positionOffset));
		float radius1 = glm::length(collider1->scale);
		float radius2 = glm::length(collider2->scale);
		if (distanceBetweenRBs <= radius1 + radius2)
		{
			return true;
		}
	}
	//If one collider is a rectangle and the other is a sphere
	else if ((collider1->colliderType == Collider::ColliderType::Sphere && collider2->colliderType == Collider::ColliderType::Rectangle) ||
		collider1->colliderType == Collider::ColliderType::Rectangle && collider2->colliderType == Collider::ColliderType::Sphere)
	{
		Collider* sphereCollider;
		Collider* rectangleCollider;

		if (collider1->colliderType == Collider::ColliderType::Sphere)
		{
			sphereCollider = collider1;
			rectangleCollider = collider2;
		}
		else
		{
			sphereCollider = collider2;
			rectangleCollider = collider1;
		}

		glm::vec3 spherePos = sphereCollider->colliderParent->componentParent->GetTranslation() + sphereCollider->positionOffset;
		glm::vec3 rectanglePos = rectangleCollider->colliderParent->componentParent->GetTranslation() - rectangleCollider->positionOffset;


		float closestX = std::min(abs(rbProjections1.x[0] - rbProjections2.x[1]), abs(rbProjections2.x[1] - rbProjections1.x[0]));
		float closestY = std::min(abs(rbProjections1.y[0] - rbProjections2.y[1]), abs(rbProjections2.y[1] - rbProjections1.y[0]));
		float closestZ = std::min(abs(rbProjections1.z[0] - rbProjections2.z[1]), abs(rbProjections2.z[1] - rbProjections1.z[0]));
		if (closestX < closestY && closestX < closestZ)
			normalDirection = glm::vec3(1, 0, 0) * (spherePos.x - rectanglePos.x);
		else if (closestY < closestX && closestY < closestZ)
			normalDirection = glm::vec3(0, 1, 0) * (spherePos.y - rectanglePos.y);
		else
			normalDirection = glm::vec3(0, 0, 1) * (spherePos.z - rectanglePos.z);

		float distanceX = abs(spherePos.x - rectanglePos.x);
		float distanceY = abs(spherePos.y - rectanglePos.y);
		float distanceZ = abs(spherePos.z - rectanglePos.z);
		
		if (distanceX > (rectangleCollider->scale.x + sphereCollider->scale.x)) { return false; }
		if (distanceY > (rectangleCollider->scale.y + sphereCollider->scale.y)) { return false; }
		if (distanceZ > (rectangleCollider->scale.z + sphereCollider->scale.z)) { return false; }
		if (distanceX <= (rectangleCollider->scale.x)) { return true; }
		if (distanceY <= (rectangleCollider->scale.y)) { return true; }
		if (distanceZ <= (rectangleCollider->scale.z)) { return true; }
		float cDist_sq = pow(distanceX - rectangleCollider->scale.x / 2, 2) + pow(distanceY - rectangleCollider->scale.y / 2, 2) + pow(distanceZ - rectangleCollider->scale.z / 2, 2);

		return cDist_sq <= sphereCollider->scale.x;
	}
	return false;
}

//Checks if a rigidbody is inside a region
bool PhysicsManager::RigidBodyInRegion(unsigned int rbIndex, PhysicsRegion& currentPhysicsRegion, bool addRB_Velocity, float gameTime)
{
	for (unsigned int i = 0; i < rigidBodies[rbIndex].GetColliders().size(); i++)
	{
		Collider* currentCollider = rigidBodies[rbIndex].GetColliderRef(i);
		//rectangle collider
		if (currentCollider->colliderType == Collider::ColliderType::Rectangle)
		{
			float rb_xPlusEdge = rigidBodies[rbIndex].componentParent->GetTranslation().x + (currentCollider->positionOffset.x + currentCollider->scale.x);
			float rb_xMinusEdge = rigidBodies[rbIndex].componentParent->GetTranslation().x + (currentCollider->positionOffset.x - currentCollider->scale.x);
			float rb_yPlusEdge = rigidBodies[rbIndex].componentParent->GetTranslation().y + (currentCollider->positionOffset.y + currentCollider->scale.y);
			float rb_yMinusEdge = rigidBodies[rbIndex].componentParent->GetTranslation().y + (currentCollider->positionOffset.y - currentCollider->scale.y);
			float rb_zPlusEdge = rigidBodies[rbIndex].componentParent->GetTranslation().z + (currentCollider->positionOffset.z + currentCollider->scale.z);
			float rb_zMinusEdge = rigidBodies[rbIndex].componentParent->GetTranslation().z - (currentCollider->positionOffset.z + currentCollider->scale.z);

			if (addRB_Velocity)
			{
				const glm::vec3 rbVelocityConstant = rigidBodies[rbIndex].GetVelocity() * gameTime;
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
			glm::vec3 rigidBodyCenter = rigidBodies[rbIndex].componentParent->GetTranslation() + currentCollider->positionOffset;
			glm::vec3 regionCenter = (currentPhysicsRegion.startBound + currentPhysicsRegion.endBound) / glm::vec3(2);
			glm::vec3 closestSpherePoint = rigidBodyCenter + (normalize(regionCenter - rigidBodyCenter) * currentCollider->scale);

			if (addRB_Velocity)
			{
				const glm::vec3 rbVelocityConstant = rigidBodies[rbIndex].GetVelocity() * gameTime;
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
				const glm::vec3 rbVelocityConstant = rigidBodies[rbIndex].GetVelocity() * gameTime;
			}
		}
		return false;
	}
	return false;
}

PhysicsManager* PhysicsManager::GetInstance()
{
	if (!instance)
		instance = new PhysicsManager();
	return instance;
}

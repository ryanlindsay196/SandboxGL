#include "RigidBody.h"
#include "FileReader.h"

RigidBody::RigidBody()
{
}


RigidBody::~RigidBody()
{
}

void RigidBody::Initialize(std::vector<std::string>& rigidBodyProperties)
{
	for (std::string rbProp : rigidBodyProperties)
	{

		rbProp.erase(std::remove(rbProp.begin(), rbProp.end(), '\t'), rbProp.end());
		rbProp.erase(std::remove(rbProp.begin(), rbProp.end(), ' '), rbProp.end());
		if (rbProp == "")
			continue;
		std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(rbProp, ":");
		if (keyValuePair.first == "PositionConstaints")
		{
			positionConstraints = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "RotationConstraints")
		{
			rotationConstraints = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Mass")
		{
			mass = strtof(keyValuePair.second.c_str(), nullptr);
		}
		else if (keyValuePair.first == "ColliderType")
		{
			colliderType = (ColliderType)std::stoi(keyValuePair.second.c_str(), nullptr);
		}
		else if (keyValuePair.first == "PositionOffset")
		{
			positionOffset = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Scale")
		{
			scale = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "UseGravity")
		{
			useGravity = std::stoi(keyValuePair.second);
		}
		else if (keyValuePair.first == "IsTrigger")
		{
			isTrigger = std::stoi(keyValuePair.second);
		}
	}
}

//TODO: DELETE
void RigidBody::Update(float gameTime)
{
}

void RigidBody::FixedUpdate(float gameTime)
{
	//if (useGravity)
	//{
	//	velocity.y += 10 * gameTime;
	//	velocity.y = std::min(10.f, velocity.y);
	//}
	if (isVelocityStored)
	{
		velocity = storedVelocity;
		isVelocityStored = false;
	}
	storedVelocity = glm::vec3(0);
	componentParent->Translate(positionConstraints * velocity * gameTime);
}

RigidBody::ColliderType RigidBody::GetColliderType()
{
	return colliderType;
}

//Get's the rigidbody's position in local space (from the componentParent)
glm::vec3 RigidBody::GetPositionOffset()
{
	return positionOffset;
}

//Get's the rigidbody's position in world space
glm::vec3 RigidBody::GetPosition()
{
	return componentParent->GetTranslation() + positionOffset;
}

glm::vec3 RigidBody::GetScale()
{
	return scale;
}

glm::vec3 RigidBody::GetVelocity()
{
	return velocity;
}

float RigidBody::GetMass()
{
	return mass;
}

float RigidBody::GetMomentumFloat()
{
	float velocityMagnitude = glm::length(velocity);
	return mass * velocityMagnitude;
}

glm::vec3 RigidBody::GetMomentumVec3()
{
	return mass * velocity;
}

float RigidBody::GetStoredMomentum()
{
	float storedVelocityMagnitude = glm::length(storedVelocity);
	return mass * storedVelocityMagnitude;
}

void RigidBody::StoreVelocity(glm::vec3 velocityToAdd)
{
	isVelocityStored = true;
	storedVelocity += velocityToAdd;
}

void RigidBody::SetVelocity(glm::vec3 newVelocity)
{
	velocity = newVelocity;
}

void RigidBody::OnCollisionEnter(Entity * entity)
{
	componentParent->OnCollisionEnter(entity);
}

void RigidBody::OnTriggerEnter(Entity * entity)
{
	componentParent->OnTriggerEnter(entity);
}

bool RigidBody::IsTrigger()
{
	return isTrigger;
}

//Calculates and returns the projections of this rigidbody on the x, y, and z axes.
RigidBody::RigidBodyProjections RigidBody::CalculateProjections(bool addVelocity, bool addStoredVelocity, float gameTime)
{
	RigidBodyProjections projections = RigidBodyProjections();

	glm::vec3 rigidBodyCenter = componentParent->GetTranslation() + positionOffset;

	projections.x[0] = glm::dot(rigidBodyCenter, glm::vec3(1, 0, 0)) + (velocity.x * addVelocity * gameTime) + (storedVelocity.x * addStoredVelocity * gameTime) - scale.x;
	projections.x[1] = glm::dot(rigidBodyCenter, glm::vec3(1, 0, 0)) + (velocity.x * addVelocity * gameTime) + (storedVelocity.x * addStoredVelocity * gameTime) + scale.x;
	projections.y[0] = glm::dot(rigidBodyCenter, glm::vec3(0, 1, 0)) + (velocity.y * addVelocity * gameTime) + (storedVelocity.y * addStoredVelocity * gameTime) - scale.y;
	projections.y[1] = glm::dot(rigidBodyCenter, glm::vec3(0, 1, 0)) + (velocity.y * addVelocity * gameTime) + (storedVelocity.y * addStoredVelocity * gameTime) + scale.y;
	projections.z[0] = glm::dot(rigidBodyCenter, glm::vec3(0, 0, 1)) + (velocity.z * addVelocity * gameTime) + (storedVelocity.z * addStoredVelocity * gameTime) - scale.z;
	projections.z[1] = glm::dot(rigidBodyCenter, glm::vec3(0, 0, 1)) + (velocity.z * addVelocity * gameTime) + (storedVelocity.z * addStoredVelocity * gameTime) + scale.z;

	return projections;
}

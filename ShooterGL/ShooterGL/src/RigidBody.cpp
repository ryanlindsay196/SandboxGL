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
			useGravity = std::stoi(keyValuePair.second.c_str());
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
	velocity += storedVelocity;
	storedVelocity = glm::vec3(0);
	componentParent->Translate(positionConstraints * velocity * gameTime);
}

RigidBody::ColliderType RigidBody::GetColliderType()
{
	return colliderType;
}

glm::vec3 RigidBody::GetPositionOffset()
{
	return positionOffset;
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

void RigidBody::StoreVelocity(glm::vec3 velocityToAdd)
{
	storedVelocity += 2.f * velocityToAdd;
}

void RigidBody::SetVelocity(glm::vec3 newVelocity)
{
	velocity = newVelocity;
}

//Calculates and returns the projections of this rigidbody on the x, y, and z axes.
RigidBody::RigidBodyProjections RigidBody::CalculateProjections()
{
	RigidBodyProjections projections = RigidBodyProjections();

	glm::vec3 rigidBodyCenter = componentParent->GetTranslation() + positionOffset;

	projections.x[0] = glm::dot(rigidBodyCenter - (scale * glm::vec3(1, 0, 0)), glm::vec3(1, 0, 0)) + velocity.x + storedVelocity.x;
	projections.x[1] = glm::dot(rigidBodyCenter + (scale * glm::vec3(1, 0, 0)), glm::vec3(1, 0, 0)) + velocity.x + storedVelocity.x;
	projections.y[0] = glm::dot(rigidBodyCenter - (scale * glm::vec3(0, 1, 0)), glm::vec3(0, 1, 0)) + velocity.y + storedVelocity.y;
	projections.y[1] = glm::dot(rigidBodyCenter + (scale * glm::vec3(0, 1, 0)), glm::vec3(0, 1, 0)) + velocity.y + storedVelocity.y;
	projections.z[0] = glm::dot(rigidBodyCenter - (scale * glm::vec3(0, 0, 1)), glm::vec3(0, 0, 1)) + velocity.z + storedVelocity.z;
	projections.z[1] = glm::dot(rigidBodyCenter + (scale * glm::vec3(0, 0, 1)), glm::vec3(0, 0, 1)) + velocity.z + storedVelocity.z;

	return projections;
}

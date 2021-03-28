#include "RigidBody.h"
#include "FileReader.h"

RigidBody::~RigidBody()
{
}

void RigidBody::Initialize(std::vector<std::string>& rigidBodyProperties, Entity* spawningEntity)
{
	spawnedBy = spawningEntity;

	for(int i = 0; i < rigidBodyProperties.size(); i++)
	//for (std::string rbProp : rigidBodyProperties)
	{
		std::string& rbProp = rigidBodyProperties[i];
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
		else if (keyValuePair.first == "Collider")
		{
			colliders.push_back(Collider());
			colliders[colliders.size() - 1].colliderParent = this;
			while (true)
			{//Loop until you find a "}" character
				i++;
				if (i < rigidBodyProperties.size())
					rbProp = rigidBodyProperties[i];
				else
					break;
				rbProp.erase(std::remove(rbProp.begin(), rbProp.end(), '\t'), rbProp.end());
				rbProp.erase(std::remove(rbProp.begin(), rbProp.end(), ' '), rbProp.end());
				if (rbProp == "EndCollider" || rbProp == "}")
					break;

				keyValuePair = GenerateKeyValuePair(rbProp, ":");
				if (keyValuePair.first == "ColliderType")
				{
					colliders[colliders.size() - 1].colliderType = (Collider::ColliderType)std::stoi(keyValuePair.second.c_str(), nullptr);
				}
				else if (keyValuePair.first == "PositionOffset")
				{
					colliders[colliders.size() - 1].positionOffset = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "Scale")
				{
					colliders[colliders.size() - 1].scale = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "IsTrigger")
				{
					colliders[colliders.size() - 1].isTrigger = std::stoi(keyValuePair.second);
				}
			}
		}
		else if (keyValuePair.first == "UseGravity")
		{
			useGravity = std::stoi(keyValuePair.second);
		}
		else if (keyValuePair.first == "Bounce")
		{
			bounce = std::stof(keyValuePair.second);
		}
	}
}

void RigidBody::Update(float gameTime)
{
	for (unsigned int i = 0; i < colliders.size(); i++)
	{
		colliders[i].Update(gameTime);
	}
}

void RigidBody::FixedUpdate(float gameTime)
{
	bool reactivateTimerActive = properties & Properties::ReactivateTimerActive;
	if (reactivateTimerActive)
	{
		if (reactivateTimer < 0)
		{
			SetIsActive(true);
		}
		else
			reactivateTimer -= gameTime;
	}

	if (useGravity)
	{
		velocity.y -= 1 * gameTime;
		velocity.y = std::max(-5.f, velocity.y);
		velocity.y = std::min(5.f, velocity.y);
	}
	if (isVelocityStored)
	{
		velocity += storedVelocity;
		isVelocityStored = false;
	}
	storedVelocity = glm::vec3(0);
	componentParent->Translate(positionConstraints * velocity * gameTime);

#ifdef DEBUG
	for (unsigned int i = 0; i < colliders.size(); i++)
		colliders[i].debugLinesColor = glm::vec3(1, 0, 0);
#endif //DEBUG
}

void RigidBody::SetSpawnedBy(Entity * spawningEntity)
{
	spawnedBy = spawningEntity;
}

Entity * RigidBody::GetSpawnedBy()
{
	return spawnedBy;
}

////Get's the rigidbody's position in local space (from the componentParent)
//glm::vec3 RigidBody::GetPositionOffset()
//{
//	return positionOffset;
//}
//
//Get's the rigidbody's position in world space
glm::vec3 RigidBody::GetPosition()
{
	return componentParent->GetTranslation();
}

//glm::vec3 RigidBody::GetScale()
//{
//	return scale;
//}

glm::vec3 RigidBody::GetVelocity()
{
	return velocity;
}

glm::vec3 RigidBody::GetStoredVelocity()
{
	return storedVelocity;
}

glm::vec3 RigidBody::GetPositionConstraints()
{
	return positionConstraints;
}

float RigidBody::GetBounce()
{
	return bounce;
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

void RigidBody::ResetStoredVelocity()
{
	storedVelocity = glm::vec3(0);
	isVelocityStored = false;
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
#ifdef DEBUG
	for (int i = 0; i < colliders.size(); i++)
		colliders[i].debugLinesColor = glm::vec3(0, 1, 0);
#endif
}

void RigidBody::OnTriggerEnter(Entity * entity)
{

}

std::vector<Collider>& RigidBody::GetColliders()
{
	return colliders;
}

Collider* RigidBody::GetColliderRef(int i)
{
	if(i < colliders.size())
		return &colliders[i];
	return nullptr;
}

void RigidBody::SetIsActive(bool newIsActive)
{
	isActive = newIsActive;
	for (Collider collider : colliders)
	{
		collider.isActive = newIsActive;
	}

}

bool RigidBody::GetIsActive()
{
	return isActive;
}

void RigidBody::SetReactivateTimer(float newReactivateTimer)
{
	reactivateTimer = newReactivateTimer;
}

unsigned int RigidBody::GetProperties()
{
	return properties;
}

void RigidBody::SetProperties(int i)
{
	properties = i;
}

void RigidBody::MoveChildReferences()
{
	for (unsigned int i = 0; i < colliders.size(); i++)
	{
		colliders[i].colliderParent = this;
	}
}

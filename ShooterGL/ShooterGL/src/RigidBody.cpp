#include "RigidBody.h"

//TODO: potentially remove pragma warning disable : 4996
#pragma warning (disable : 4996)

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
	}
}

//TODO: DELETE
void RigidBody::Update(float gameTime)
{
}

void RigidBody::FixedUpdate(float gameTime)
{
	velocity.y += 10 * gameTime;
	velocity.y = std::min(10.f, velocity.y);
	componentParent->Translate(positionConstraints * velocity * gameTime);
}


glm::vec3 RigidBody::ParseVector(std::string line)
{
	if (line.find(",") == line.npos)
		return glm::vec3(strtof((char*)line.c_str(), nullptr));

	float x, y, z;
	std::string xLine, yLine, zLine;
	xLine = yLine = zLine = line;

	xLine = strtok((char*)xLine.c_str(), ",");

	yLine = yLine.substr(yLine.find_first_of(",") + 1);
	yLine = strtok((char*)yLine.c_str(), ",");

	zLine = zLine.substr(zLine.find_last_of(",") + 1);
	//zLine = strtok((char*)zLine.c_str(), ",");

	x = strtof((char*)xLine.c_str(), nullptr);
	y = strtof((char*)yLine.c_str(), nullptr);
	z = strtof((char*)zLine.c_str(), nullptr);
	return glm::vec3(x, y, z);
}

std::pair<std::string, std::string> RigidBody::GenerateKeyValuePair(std::string line, std::string delimiter)
{
	std::pair<std::string, std::string> newKeyValuePair;
	newKeyValuePair.second = line.substr(line.find_first_of(":") + 1);
	newKeyValuePair.first = strtok((char*)line.c_str(), delimiter.c_str());
	return newKeyValuePair;
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

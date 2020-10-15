#pragma once
#include "Component.h"
#include <algorithm>
#include <string>
class RigidBody : public Component
{
public:
	enum ColliderType { Sphere, Rectangle, Mesh };
private:
	//TODO: Make fixedUpdateTime a globally set variable. Maybe set it in a ProjectSettings file?
	//float fixedUpdateTime, fixedUpdateTimer;
	float mass;
	glm::vec3 velocity;
	glm::vec3 positionConstraints, rotationConstraints;
	glm::vec3 positionOffset, scale;

	ColliderType colliderType;
public:
	RigidBody();
	~RigidBody();
	void Initialize(std::vector<std::string>& rigidBodyProperties);
	void Update(float gameTime) override;
	void FixedUpdate(float gameTime);
	glm::vec3 ParseVector(std::string line);
	std::pair<std::string, std::string> GenerateKeyValuePair(std::string line, std::string delimiter);
	ColliderType GetColliderType();
	glm::vec3 GetPositionOffset();
	glm::vec3 GetScale();
	glm::vec3 GetVelocity();
};


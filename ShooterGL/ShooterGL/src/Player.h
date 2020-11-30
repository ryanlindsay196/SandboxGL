#pragma once
#include <string>
#include "Component.h"

class ControllerManager;
class EntityManager;

struct PlayerInfo
{
	std::string name;
	std::string portrait;
	float health, currentHealth;
	float weight;
	float energy, currentEnergy;
	float energyRecovery;
	float speed;
	float shield, currentShield;
	float shieldRecovery;
};

enum ProjectileType { lob, skillshot };

struct ProjectileData
{
	ProjectileType projectileType;
	float damage;
	glm::vec3 positionOffset;
	glm::vec3 rotationAxis;
	glm::vec3 rotationAngle;
	float homingAngle;
	float homingDuration, homingTimer;
	ProjectileData* spawnOnHit;
	float knockBack;
	glm::vec3 startVelocity, startAcceleration;
};

struct HitBox
{
	float damage;
	glm::vec3 hitBoxSize;
	float duration;
};

struct FrameData 
{
	std::vector<HitBox> frames;
};

struct AttackData
{
	//Macro macro;
	float energyUsage;
	ProjectileData* projectileData;
};

class Player : public Component
{
public:
	Player();
	void Initialize(ControllerManager* controllerManager, Entity* newParent, EntityManager* newEntityManager);
	void Update(float gameTime) override;
	void OnCollisionEnter(Entity* entity) override;

	void SetController(Controller* newController);
private:
	std::vector<ProjectileData> projectileData;
	PlayerInfo playerInfo;
	Controller* controller;
	EntityManager* entityManager;
};
#pragma once
#include <vector>
#include "glm.hpp"
#include "../WorldComponent.h"
class Shader;
class ObjectManager;
class ShaderManager;

struct Force
{
	glm::vec3 Position;
	float strength;
	float range;
};

struct Particle {
	glm::vec3 Position, Velocity, Acceleration;
	glm::vec4 Color;
	glm::vec3 Size;
	float Life, StartLife;

	Particle()
		:Position(0.0f), Velocity(1.0f), Color(1.0f), Life(1.0f) {}
};

enum SpawnerShape { sphere, rectangle };

struct SpawnerSettings
{
	float spawnsPerSecond;
	unsigned int particlesPerSpawn;
	float minLifeTime;
	float maxLifeTime;
	unsigned int maxParticles = 500;
	SpawnerShape spawnerShape;
	glm::vec3 spawnerDimensions;
	glm::vec3 spawnerOffset;
	SpawnerShape velocityShape;
	glm::vec3 velocityDimensions;
	glm::vec3 velocityOffset;
	SpawnerShape accelerationShape;
	glm::vec3 accelerationDimensions;
	glm::vec3 accelerationOffset;
	glm::vec3 startSize;
	glm::vec3 endSize;
	std::vector<Force> forces;
};

class ParticleEmitter : public WorldComponent
{
private:
	SpawnerSettings spawnerSettings;
public:
	void Initialize(ObjectManager* objectManager, char* particlePath);
	void LoadParticleSettings(char* particlePath);
	virtual void Update(float gameTime) override;
	void Render();

private:
	std::vector<Particle> particles;
	unsigned int VBO, VAO;
	unsigned int lastUsedParticle = 0;
	unsigned int FirstUnusedParticle();
	void RespawnParticle(Particle *particle);
	Shader* m_shader;
	ObjectManager* m_objectManager;
	float timeUntilSpawn;
};
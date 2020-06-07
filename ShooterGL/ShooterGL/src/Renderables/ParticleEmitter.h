#pragma once
#include <vector>
#include "glm.hpp"
#include "../WorldComponent.h"
class Shader;
class ObjectManager;
class ShaderManager;

struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	float Life;

	Particle()
		:Position(0.0f), Velocity(1.0f), Color(1.0f), Life(1.0f) {}
};

class ParticleEmitter : public WorldComponent
{
private:
	//TODO: Load from file
	unsigned int totalParticleCount = 500;
	//TODO: Load from file
	unsigned int newParticlesPerSecond = 2;
public:
	void Initialize(ObjectManager* objectManager, char* materialPath);
	virtual void Update(float gameTime) override;
	void Render();

private:
	std::vector<Particle> particles;
	unsigned int VBO, VAO;
	unsigned int lastUsedParticle = 0;
	unsigned int FirstUnusedParticle();
	void RespawnParticle(Particle *particle, glm::vec3 offset);
	Shader* m_shader;
	ObjectManager* m_objectManager;
};
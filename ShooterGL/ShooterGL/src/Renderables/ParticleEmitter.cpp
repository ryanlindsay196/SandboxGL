#include "ParticleEmitter.h"
#include "Shader.h"
#include "ManagerClasses/ShaderManager.h"
//#include "GLFW/glfw3.h"
#include <glad/glad.h>

void ParticleEmitter::Initialize(ObjectManager * objectManager, char* materialPath)
{
	float particle_quad[] = {
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	//fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	//set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	m_shader = objectManager->shaderManager->LoadNewShader(materialPath, objectManager);
	//create the correct amount of particle instances
	for (unsigned int i = 0; i < totalParticleCount; ++i)
	{
		particles.push_back(Particle());
	}

}

void ParticleEmitter::Update(float gameTime)
{
	//add new particles
	for (unsigned int i = 0; i < newParticlesPerSecond * gameTime; ++i)
	{
		int unusedParticle = FirstUnusedParticle();
		RespawnParticle(&particles[unusedParticle], glm::vec3(0, 0, 0));
		
		//update all particles
		for (unsigned int i = 0; i < totalParticleCount; ++i)
		{
			Particle &p = particles[i];
			p.Life -= gameTime; // reduce life
			if (p.Life > 0.0f)
			{ // particle is alife, thus update
				p.Position -= p.Velocity * gameTime;
				p.Color.a -= gameTime * 2.5f;
			}
		}
	}
}

void ParticleEmitter::Render()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_shader->UseShader();
	for (Particle particle : particles)
	{
		if (particle.Life > 0.0f)
		{
			m_shader->SetShaderUniform_vec2((char*)"offset", particle.Position.x, particle.Position.y);
			m_shader->SetShaderUniform_vec4((char*)"color", particle.Color.r, particle.Color.g, particle.Color.b, particle.Color.a);
			m_shader->BindTextures();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

unsigned int ParticleEmitter::FirstUnusedParticle()
{
	//search from last used particle, this will usually return almost instantly
	for (unsigned int i = lastUsedParticle; i < totalParticleCount; ++i)
	{
		if (particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//otherwise, do a linear search
	for (unsigned int i = 0; i < lastUsedParticle; ++i) {
		if (particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//override first particle if all others are alive
	lastUsedParticle = 0;
	return 0;
}

void ParticleEmitter::RespawnParticle(Particle * particle, glm::vec3 offset)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle->Position = componentParent->GetTranslation() + random + offset;
	particle->Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle->Life = 1.0f;
	//TODO: Load velocity values from file
	particle->Velocity = glm::vec3(rColor) * 2.1f;
}

#include "ParticleEmitter.h"
#include "Shader.h"
#include "ManagerClasses/ShaderManager.h"
//#include "GLFW/glfw3.h"
#include <glad/glad.h>
#include "ManagerClasses/ObjectManager.h"
#include "ManagerClasses/CameraManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <gtc/random.hpp>
//TODO: potentially remove pragma warning disable : 4996
#pragma warning (disable : 4996)

void ParticleEmitter::Initialize(ObjectManager * objectManager, char* particlePath)
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

	m_objectManager = objectManager;
	LoadParticleSettings(particlePath);
	//create the correct amount of particle instances
	for (unsigned int i = 0; i < spawnerSettings.maxParticles; ++i)
	{
		particles.push_back(Particle());
	}

}

void ParticleEmitter::LoadParticleSettings(char * particlePath)
{
	std::ifstream materialFile(particlePath);

	std::string line;
	while (getline(materialFile, line))
	{
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
		std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(line, ":");
		if (keyValuePair.first == "material")
			m_shader = m_objectManager->shaderManager->LoadNewShader(keyValuePair.second, m_objectManager);
		else if (keyValuePair.first == "spawnsPerSecond")
			spawnerSettings.spawnsPerSecond = strtof(keyValuePair.second.c_str(), nullptr);
		else if (keyValuePair.first == "particlesPerSpawn")
			spawnerSettings.particlesPerSpawn = strtof(keyValuePair.second.c_str(), nullptr);
		else if (keyValuePair.first == "minLifeTime")
			spawnerSettings.minLifeTime = strtof(keyValuePair.second.c_str(), nullptr);
		else if (keyValuePair.first == "maxLifeTime")
			spawnerSettings.maxLifeTime = strtof(keyValuePair.second.c_str(), nullptr);
		else if (keyValuePair.first == "maxParticles")
			spawnerSettings.maxParticles = strtof(keyValuePair.second.c_str(), nullptr);
		else if (keyValuePair.first == "spawnerShape")
		{
			if (keyValuePair.second == "sphere")
				spawnerSettings.spawnerShape = SpawnerShape::sphere;
			else if (keyValuePair.second == "rectangle")
				spawnerSettings.spawnerShape = SpawnerShape::rectangle;
		}
		else if (keyValuePair.first == "spawnerDimensions")
			spawnerSettings.spawnerDimensions = ParseVector(keyValuePair.second);
		else if (keyValuePair.first == "spawnerOffset")
			spawnerSettings.spawnerOffset = ParseVector(keyValuePair.second);
		else if (keyValuePair.first == "velocityShape")
		{
			if (keyValuePair.second == "sphere")
				spawnerSettings.velocityShape = SpawnerShape::sphere;
			else if (keyValuePair.second == "rectangle")
				spawnerSettings.velocityShape = SpawnerShape::rectangle;
		}
		else if (keyValuePair.first == "velocityDimensions")
			spawnerSettings.velocityDimensions = ParseVector(keyValuePair.second);
		else if (keyValuePair.first == "velocityOffset")
			spawnerSettings.velocityOffset = ParseVector(keyValuePair.second);
		else if (keyValuePair.first == "accelerationShape")
		{
			if (keyValuePair.second == "sphere")
				spawnerSettings.accelerationShape = SpawnerShape::sphere;
			else if (keyValuePair.second == "rectangle")
				spawnerSettings.accelerationShape = SpawnerShape::rectangle;
		}
		else if (keyValuePair.first == "accelerationDimensions")
			spawnerSettings.accelerationDimensions = ParseVector(keyValuePair.second);
		else if (keyValuePair.first == "accelerationOffset")
			spawnerSettings.accelerationOffset = ParseVector(keyValuePair.second);
		else if (keyValuePair.first == "startSize")
			spawnerSettings.startSize = ParseVector(keyValuePair.second);
		else if (keyValuePair.first == "endSize")
			spawnerSettings.endSize = ParseVector(keyValuePair.second);
	}
	materialFile.close();
}

std::pair<std::string, std::string> ParticleEmitter::GenerateKeyValuePair(std::string line, std::string delimiter)
{
	std::pair<std::string, std::string> newKeyValuePair;
	newKeyValuePair.second = line.substr(line.find_first_of(":") + 1);
	newKeyValuePair.first = strtok((char*)line.c_str(), delimiter.c_str());
	return newKeyValuePair;
}

glm::vec3 ParticleEmitter::ParseVector(std::string line)
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

void ParticleEmitter::Update(float gameTime)
{
	timeUntilSpawn -= gameTime;
	//add new particles
	if (timeUntilSpawn <= 0)
	{
		timeUntilSpawn = 1 / spawnerSettings.spawnsPerSecond;
		for (unsigned int i = 0; i < spawnerSettings.particlesPerSpawn; ++i)
		{
			int unusedParticle = FirstUnusedParticle();
			RespawnParticle(&particles[unusedParticle]);
		}

	}
	
	//update all particles
	for (unsigned int i = 0; i < spawnerSettings.maxParticles; ++i)
	{
		Particle &p = particles[i];
		p.Life -= gameTime; // reduce life
		if (p.Life > 0.0f)
		{ // particle is alive, thus update
			p.Position -= p.Velocity * gameTime;
			//p.Color.a -= gameTime * 2.5f;
			p.Velocity -= p.Acceleration * gameTime;
			p.Size = (spawnerSettings.startSize * (p.Life / p.StartLife)) +
				(spawnerSettings.endSize * (1.f-(p.Life / p.StartLife)));
		}
	}
}

void ParticleEmitter::Render()
{
	glCullFace(GL_FRONT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_shader->UseShader();
	m_shader->SetShaderUniform_mat4fv((char*)"projection", m_objectManager->cameraManager->GetCamera(0)->projectionMatrix);
	m_shader->SetShaderUniform_mat4fv((char*)"view", m_objectManager->cameraManager->GetCamera(0)->viewMatrix);
	for (Particle particle : particles)
	{
		if (particle.Life > 0.0f)
		{
			m_shader->SetShaderUniform_mat4fv((char*)"position", glm::translate(glm::mat4(1), particle.Position));
			//m_shader->SetShaderUniform_mat4fv((char*)"position", glm::translate(glm::mat4(1), glm::vec3(0)));
			//m_shader->SetShaderUniform_vec3((char*)"offset", particle.Position.x, particle.Position.y, particle.Position.z);
			//m_shader->SetShaderUniform_vec4((char*)"color", particle.Color.r, particle.Color.g, particle.Color.b, particle.Color.a);
			m_shader->SetShaderUniform_vec2((char*)"size", particle.Size.x, particle.Size.y);
			m_shader->BindTextures();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
}

unsigned int ParticleEmitter::FirstUnusedParticle()
{
	//search from last used particle, this will usually return almost instantly
	for (unsigned int i = lastUsedParticle; i < spawnerSettings.maxParticles; ++i)
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

void ParticleEmitter::RespawnParticle(Particle * particle)
{
	//float random = 0 * ((rand() % 100) - 50) / 10.0f;
	//float rColor = 0.5f + ((rand() % 100) / 100.0f);

	//Randomly positioned inside the area of the shape (rect/sphere)
	particle->Position = -componentParent->GetTranslation() +
		(glm::sphericalRand(glm::length(spawnerSettings.spawnerDimensions)) * spawnerSettings.spawnerDimensions) +
		spawnerSettings.spawnerOffset;
	//particle->Position = -componentParent->GetTranslation();
	//particle->Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle->Life = glm::linearRand(spawnerSettings.minLifeTime, spawnerSettings.maxLifeTime);

	particle->Velocity = (glm::sphericalRand(glm::length(spawnerSettings.velocityDimensions)) * spawnerSettings.velocityDimensions) +
		spawnerSettings.velocityOffset;


	particle->Acceleration = (glm::sphericalRand(glm::length(spawnerSettings.accelerationDimensions)) * spawnerSettings.accelerationDimensions) +
		spawnerSettings.accelerationOffset;

	particle->StartLife = particle->Life;
}

#include "EntityManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "ControllerManager.h"
#include "ModelManager.h"
#include "LightManager.h"
#include "Renderables/ParticleEmitter.h"

#include "GLFW/glfw3.h"

#include <fstream>
#include <sstream>
#include <algorithm>

void EntityManager::Initialize(ObjectManager * in_objectManager)
{
	objectManager = in_objectManager;
}

void EntityManager::LoadScene(std::string scenePath)
{
	std::ifstream sceneFile(scenePath);
	std::string line;
	while (getline(sceneFile, line))
	{
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
		if (line == "")
			continue;
		std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(line, ":");

		if (keyValuePair.first == "PREFAB")
		{
			EntityProperties* propsToLoad = LoadPrefab(keyValuePair.second);
			glm::vec3 newPosition;
			glm::vec3 newEulerAngles;
			float newRotationAngle = 0.0f;
			glm::vec3 newScale;
			while (getline(sceneFile, line))
			{
				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				std::pair<std::string, std::string> propertyPair = GenerateKeyValuePair(line, ":");
				if (line == "}")
					break;
				else if (propertyPair.first == "Position")
					newPosition = ParseVector(propertyPair.second);
				else if (propertyPair.first == "EulerAngles")
					newEulerAngles = ParseVector(propertyPair.second);
				else if (propertyPair.first == "RotationAngle")
					newRotationAngle = strtof(propertyPair.second.c_str(), nullptr);
				else if (propertyPair.first == "Scale")
					newScale = ParseVector(propertyPair.second);
			}
			InstantiateEntity(propsToLoad, newPosition, newEulerAngles, newRotationAngle, newScale, nullptr);
		}
	}

}

EntityManager::EntityProperties* EntityManager::LoadPrefab(std::string prefabPath)
{
	std::ifstream prefabFile(prefabPath);
	std::string line;

	EntityProperties* entityPropertiesToUse = LoadProperties(prefabPath);

	if (!entityPropertiesToUse->isDataLoaded)
	{
		while (getline(prefabFile, line))
		{
			std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(line, ":");
			if (line == "Components")
			{
				getline(prefabFile, line);//This skips the "{" after the Components property
				while (getline(prefabFile, line))
				{
					line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
					line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
					if (line != "}" && line != "{")
					{
						entityPropertiesToUse->componentProperties.push_back(std::vector<std::string>());
						entityPropertiesToUse->componentNames.push_back(line);
					}
					else if(line == "{")
					{
						while (getline(prefabFile, line))
						{//Load component properties
							line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
							line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
							if (line == "}")
								break;
							else 
								entityPropertiesToUse->componentProperties[entityPropertiesToUse->componentProperties.size() - 1].push_back(line);
						}
					}
					else if(line == "}")
						break;

				}
			}
			else if (line == "Tags")
			{
				while (getline(prefabFile, line))
				{
					line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
					line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
					if (line == "{")
						continue;
					else if (line == "}")
						break;
					else
						entityPropertiesToUse->tags.push_back(line);
				}
			}
		}
		entityPropertiesToUse->isDataLoaded = true;
	}
	return entityPropertiesToUse;
}

EntityManager::EntityProperties* EntityManager::LoadProperties(std::string prefabPath)
{
	auto entityProps = entityPropertiesMap.find(prefabPath);
	if (entityProps == entityPropertiesMap.end())
	{
		EntityProperties* newEntityProps = new EntityProperties();
		//newEntityProps->Initialize(in_objectManager, (char*)"", (char*)"", (char*)materialPath.c_str());

		std::pair<std::string, EntityProperties*> newEntityPropsPair(prefabPath, newEntityProps);
		entityPropertiesMap.insert(newEntityPropsPair);
		return newEntityPropsPair.second;// shaders.find(shaderPath);
	}
	return entityPropertiesMap[prefabPath];
}

void EntityManager::InstantiateEntity(EntityProperties* entityProperties, glm::vec3 startPos, glm::vec3 startEulerAngles, float rotationAngle, glm::vec3 startScale, Entity* parent)
{
	Entity* entity = new Entity();
	entity->Instantiate(startPos, startEulerAngles, rotationAngle, startScale, parent);
	entities.push_back(entity);

	for (unsigned int i = 0; i < entityProperties->componentNames.size(); i++)
	{
		if (entityProperties->componentNames[i] == "Camera")
		{
			entities[entities.size() - 1]->AddComponent(objectManager->cameraManager->CreateCamera(entities[entities.size() - 1]));
		}
		else if (entityProperties->componentNames[i] == "Model")
		{
			ModelData modelTransform = ModelData();
			modelTransform.ReadModelData(entityProperties->componentProperties[i]);
			entities[entities.size() - 1]->AddComponent(objectManager->modelManager->LoadModel((char*)modelTransform.modelPath.c_str(), modelTransform.materialPath, modelTransform.position, modelTransform.rotationAxis, modelTransform.rotationAngle, modelTransform.scale));
		}
		else if (entityProperties->componentNames[i] == "ParticleSystem")
		{
			ParticleEmitter* newParticleEmitter = new ParticleEmitter();
			std::pair<std::string, std::string> particleKeyValue = GenerateKeyValuePair(entityProperties->componentProperties[i][0], ":");
			newParticleEmitter->Initialize(objectManager, (char*)particleKeyValue.second.c_str());
			entities[entities.size() - 1]->AddComponent(newParticleEmitter);
		}
		else if (entityProperties->componentNames[i] == "Controller")
		{
			entities[entities.size() - 1]->AddComponent(objectManager->controllerManager->CreateController(nullptr));
		}
		else if (entityProperties->componentNames[i] == "Light:Point")
		{
			PointLightData pointLightData = PointLightData();
			pointLightData.ReadPointLightData(entityProperties->componentProperties[i]);
			entities[entities.size() - 1]->AddComponent(objectManager->lightManager->AddLight(pointLightData.position, pointLightData.rotationAxis, pointLightData.rotationAngle, pointLightData.scale, pointLightData.ambient, pointLightData.specular, pointLightData.diffuse));
		}
		else if (entityProperties->componentNames[i] == "RigidBody")
		{
			RigidBody* newRigidBody = new RigidBody();
			newRigidBody->Initialize(entityProperties->componentProperties[i]);
			entities[entities.size() - 1]->AddComponent(newRigidBody);
			newRigidBody->componentParent = entities[entities.size() - 1];
			objectManager->physicsManager->InitializeRigidBody(newRigidBody, 0.f);
		}
	}
}

void EntityManager::Update(float gameTime)
{
	for (Entity* entity : entities)
	{
		entity->Update(gameTime);
	}
}

void EntityManager::Render()
{
	for (Entity* e : entities)
	{
		e->Render();
	}
}

Entity* EntityManager::GetEntity(int i)
{
	if ((unsigned int)i < entities.size())
		return entities[i];
	return nullptr;
}
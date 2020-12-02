#include "EntityManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "ControllerManager.h"
#include "ModelManager.h"
#include "LightManager.h"
#include "PlayerManager.h"
#include "Player.h"
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
	for (auto entity : entities)
	{
		delete(entity);
	}
	entities.clear();
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
			EntityProperties* propsToLoad = LoadEntityFromFile(keyValuePair.second);
			glm::vec3 newPosition;
			glm::vec3 newEulerAngles;
			float newRotationAngle = 0.0f;
			glm::vec3 newScale;
			while (getline(sceneFile, line))
			{
				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				std::pair<std::string, std::string> propertyPair = GenerateKeyValuePair(line, ":");
				if (line == "{")
					continue;
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

//Loads entity properties from a prefab file
EntityManager::EntityProperties* EntityManager::LoadEntityFromFile(std::string prefabPath)
{
	std::ifstream prefabFile(prefabPath);
	std::string line;

	EntityProperties* entityPropertiesToUse = LoadProperties(prefabPath);

	//Fill entityPropertiesToUse if it's not filled
	if (!entityPropertiesToUse->isDataLoaded)
	{
		while (getline(prefabFile, line))
		{
			std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(line, ":");
			if (line == "Components")
			{
				int scopeDepth = 0;
				getline(prefabFile, line);//This skips the "{" after the Components property
				while (getline(prefabFile, line))
				{
					line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
					line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
					//If we've encountered a new property for the current entity
					if (line != "}" && line != "{")
					{
						//Add a new set of entityData
						entityPropertiesToUse->entityData.push_back(EntityData());
						//entityPropertiesToUse->entityData[entityPropertiesToUse->entityData.size() - 1].componentProperties.push_back(std::string());
						entityPropertiesToUse->entityData[entityPropertiesToUse->entityData.size() - 1].componentName = line;
					}
					else if (line == "{")
					{
						scopeDepth++;
						while (getline(prefabFile, line))
						{//Load component properties
							line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
							line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
							if (line == "}")
							{
								scopeDepth--;
								if(scopeDepth == 0)
								break;
							}
							else if (line == "{")
								scopeDepth++;
							else
								entityPropertiesToUse->entityData[entityPropertiesToUse->entityData.size() - 1].componentProperties.push_back(line);
						}
					}
					else if (line == "}")
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

		std::pair<std::string, EntityProperties*> newEntityPropsPair(prefabPath, newEntityProps);
		entityPropertiesMap.insert(newEntityPropsPair);
		return newEntityProps;
	}
	return entityPropertiesMap[prefabPath];
}

//Add an entity into the scene
Entity* EntityManager::InstantiateEntity(EntityProperties* entityProperties, glm::vec3 startPos, glm::vec3 angleAxis, float rotationAngle, glm::vec3 startScale, Entity* parent)
{
	Entity* entity = new Entity();
	entity->Instantiate(startPos, angleAxis, rotationAngle, startScale, parent);
	entities.push_back(entity);

	for (EntityData entityData : entityProperties->entityData)
	{
		if (entityData.componentName == "Camera")
		{
			entity->AddComponent(objectManager->cameraManager->CreateCamera(entity));
		}
		else if (entityData.componentName == "Model")
		{
			ModelData modelTransform = ModelData();
			modelTransform.ReadModelData(entityData.componentProperties);
			entity->AddComponent(objectManager->modelManager->LoadModel(modelTransform.modelPath, modelTransform.materialPath, modelTransform.position, modelTransform.rotationAxis, modelTransform.rotationAngle, modelTransform.scale));
		}
		else if (entityData.componentName == "ParticleSystem")
		{
			ParticleEmitter* newParticleEmitter = new ParticleEmitter();
			std::pair<std::string, std::string> particleKeyValue = GenerateKeyValuePair(entityData.componentProperties[0], ":");
			newParticleEmitter->Initialize(objectManager, (char*)particleKeyValue.second.c_str());
			entity->AddComponent(newParticleEmitter);
		}
		else if (entityData.componentName == "Controller")
		{
			entity->AddComponent(objectManager->controllerManager->CreateController(nullptr));

			Player* latestPlayer = objectManager->playerManager->GetPlayer(objectManager->playerManager->TotalPlayers() - 1);
			//if the player was added before the controller
			//if not, then attach the controller inside the player script instead
			if (latestPlayer != nullptr && latestPlayer->componentParent == entity)
			{
				//Set the controller in the current entity's player component to the newly created controller
				latestPlayer->SetController(objectManager->controllerManager->GetController(objectManager->controllerManager->TotalControllers() - 1));
			}
		}
		else if (entityData.componentName == "Light:Point")
		{
			entity->AddComponent(objectManager->lightManager->AddLight(entityData.componentProperties));
		}
		else if (entityData.componentName == "RigidBody")
		{
			RigidBody* newRigidBody = new RigidBody();
			newRigidBody->Initialize(entityData.componentProperties, nullptr);
			entity->AddComponent(newRigidBody);
			newRigidBody->componentParent = entity;
			objectManager->physicsManager->InitializeRigidBody(newRigidBody, 0.f);
		}
		else if (entityData.componentName == "Player")
		{
			Player* player = new Player();
			player->Initialize(objectManager->controllerManager, entity, this, );
			entity->AddComponent(player);
			objectManager->playerManager->AddPlayer(player);
		}
		else
		{
			std::cout << "EntityManager::InstantiateEntity(...)::Component " << entityData.componentName << " is not supported." << std::endl;
		}
	}

	return entity;
}

void EntityManager::Update(float gameTime)
{
	for(int i = 0; i < entities.size(); i++)
	{
		entities[i]->Update(gameTime);
	}
}

void EntityManager::Render(LightManager* lightManager)
{
	for (Entity* e : entities)
	{
		e->Render(lightManager);
	}
}

Entity* EntityManager::GetEntity(int i)
{
	if ((unsigned int)i < entities.size())
		return entities[i];
	return nullptr;
}
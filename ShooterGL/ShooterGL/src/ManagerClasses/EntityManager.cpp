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

//TODO: potentially remove pragma warning disable : 4996
#pragma warning (disable : 4996)

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


glm::vec3 EntityManager::ParseVector(std::string line)
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

std::pair<std::string, std::string> EntityManager::GenerateKeyValuePair(std::string line, std::string delimiter)
{
	std::pair<std::string, std::string> newKeyValuePair;
	newKeyValuePair.second = line.substr(line.find_first_of(delimiter) + 1);
	newKeyValuePair.first = strtok((char*)line.c_str(), delimiter.c_str());
	return newKeyValuePair;
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
	entities.push_back(new Entity());
	entities[entities.size() - 1]->Instantiate(startPos, startEulerAngles, rotationAngle, startScale, parent);

	for (unsigned int i = 0; i < entityProperties->componentNames.size(); i++)
	{
		if (entityProperties->componentNames[i] == "Camera")
		{
			entities[entities.size() - 1]->AddComponent(objectManager->cameraManager->CreateCamera(entities[entities.size() - 1]));
		}
		else if (entityProperties->componentNames[i] == "Model")
		{
			ModelData modelTransform = ReadModelData(entityProperties->componentProperties[i]);
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
			PointLightData pointLightData = ReadPointLightData(entityProperties->componentProperties[i]);
			entities[entities.size() - 1]->AddComponent(objectManager->lightManager->AddLight(pointLightData.position, pointLightData.rotationAxis, pointLightData.rotationAngle, pointLightData.scale, pointLightData.ambient, pointLightData.specular, pointLightData.diffuse));
		}
		else if (entityProperties->componentNames[i] == "RigidBody")
		{
			RigidBody* newRigidBody = new RigidBody();
			newRigidBody->Initialize(entityProperties->componentProperties[i]);
			entities[entities.size() - 1]->AddComponent(newRigidBody);
			newRigidBody->componentParent = entities[entities.size() - 1];
			objectManager->physicsManager->AddRigidBody(newRigidBody, 0.f);
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

EntityManager::ModelData EntityManager::ReadModelData(std::vector<std::string> transformDataString)
{
	ModelData modelData = ModelData();


	for (unsigned int j = 0; j < transformDataString.size(); j++)
	{
		transformDataString[j].erase(std::remove(transformDataString[j].begin(), transformDataString[j].end(), '\t'), transformDataString[j].end());
		transformDataString[j].erase(std::remove(transformDataString[j].begin(), transformDataString[j].end(), ' '), transformDataString[j].end());
		std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(transformDataString[j], ":");
		if (keyValuePair.first == "Path")
		{
			modelData.modelPath = keyValuePair.second;
		}
		else if (keyValuePair.first == "PositionOffset")
		{
			modelData.position = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "EulerAngles")
		{
			modelData.rotationAxis = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "RotationAngle")
		{
			modelData.rotationAngle = strtof((char*)keyValuePair.second.c_str(), nullptr);
		}
		else if (keyValuePair.first == "ScaleOffset")
		{
			modelData.scale = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Material")
		{
			modelData.materialPath = keyValuePair.second;
		}
	}
	return modelData;
}

EntityManager::PointLightData EntityManager::ReadPointLightData(std::vector<std::string> dataString)
{
	PointLightData pointLightData = PointLightData();
	for (unsigned int j = 0; j < dataString.size(); j++)
	{
		if (dataString[j] == "")
			continue;
		dataString[j].erase(std::remove(dataString[j].begin(), dataString[j].end(), '\t'), dataString[j].end());
		dataString[j].erase(std::remove(dataString[j].begin(), dataString[j].end(), ' '), dataString[j].end());
		std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(dataString[j], ":");
		if (keyValuePair.first == "Ambient")
		{
			pointLightData.ambient = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Specular")
		{
			pointLightData.specular = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Diffuse")
		{
			pointLightData.diffuse = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "Position")
		{
			pointLightData.position = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "EulerAngles")
		{
			pointLightData.rotationAxis = ParseVector(keyValuePair.second);
		}
		else if (keyValuePair.first == "RotationAngle")
		{
			pointLightData.rotationAngle = strtof(keyValuePair.second.c_str(), nullptr);
		}
		else if (keyValuePair.first == "Scale")
		{
			pointLightData.scale = ParseVector(keyValuePair.second);
		}
	}

	return pointLightData;
}

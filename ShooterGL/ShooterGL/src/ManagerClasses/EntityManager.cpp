#include "EntityManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "ControllerManager.h"

#include "GLFW/glfw3.h"

#include <fstream>
#include <sstream>
#include <algorithm>

//TODO: potentially remove pragma warning disable : 4996
#pragma warning (disable : 4996)

void EntityManager::Initialize(ObjectManager * in_objectManager, std::string scenePath)
{
	//TODO: Loop through all files in the entities folder to load entity properties
	objectManager = in_objectManager;


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
			glm::vec3 newRotationAxis;
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
				else if (propertyPair.first == "RotationAxis")
					newRotationAxis = ParseVector(propertyPair.second);
				else if (propertyPair.first == "RotationAngle")
					newRotationAngle = strtof(propertyPair.second.c_str(), nullptr);
				else if(propertyPair.first == "Scale")
					newScale = ParseVector(propertyPair.second);
			}
			InstantiateEntity(*propsToLoad, newPosition, newRotationAxis, newRotationAngle, newScale, nullptr);
		}
	}
	//InstantiateEntity(EntityProperties());
	//InstantiateEntity(EntityProperties());
	//InstantiateEntity(EntityProperties());
	//InstantiateEntity(EntityProperties());
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
	newKeyValuePair.second = line.substr(line.find_first_of(":") + 1);
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
			//TODO: Read through prefab file and update entity properties based on what's in the prefab
			if (line == "Components")
			{
				getline(prefabFile, line);//This skips the "{" after the Components property
				while (getline(prefabFile, line))
				{
					line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
					line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
					if (line != "}" && line != "{")
						entityPropertiesToUse->componentNames.push_back(line);
					else if(line == "{")
					{
						entityPropertiesToUse->componentProperties.push_back(std::vector<std::string>());
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
	for (auto& x : entityPropertiesMap)
	{//TODO: Replace this with instant lookup
		if (x.first == prefabPath)
			return x.second;
	}
	return nullptr;
}

//TODO: Make private and use the entity properties for properties, and load all entities from the scene file
void EntityManager::InstantiateEntity(EntityProperties entityProperties, glm::vec3 startPos, glm::vec3 startRotationAxis, float rotationAngle, glm::vec3 startScale, Entity* parent)
{
	//TODO: Instantiate entity based on entity properties unordered map
	entities.push_back(new Entity());
	entities[entities.size() - 1]->Instantiate(startPos, startRotationAxis, rotationAngle, startScale, parent);
	objectManager->cameraManager->CreateCamera(entities[entities.size() - 1]);
	entities[entities.size() - 1]->AddComponent(objectManager->controllerManager->CreateController(entities[entities.size() - 1]));
}

void EntityManager::Update(float gameTime)
{
	for (Entity* entity : entities)
	{
		entity->Update(gameTime);
	}
}

Entity* EntityManager::GetEntity(int i)
{
	if ((unsigned int)i < entities.size())
		return entities[i];
	return nullptr;
}

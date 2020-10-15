#pragma once
#include <unordered_map>
#include "Entity.h"
#include <vector>
#include "../RigidBody.h"
#include "PhysicsManager.h"
#include "FileReader.h"

class ObjectManager;

class EntityManager
{
private:
	ObjectManager* objectManager;
public://TODO: Implement
	struct EntityProperties
	{
		bool isDataLoaded = false;
		//components
		std::vector<std::string> componentNames;
		std::vector<std::vector<std::string>> componentProperties;
		//tags
		std::vector<std::string> tags;
		////children
		//std::vector<std::string> childEntities;
	};

	struct ModelData
	{
		std::string modelPath;
		std::string materialPath;
		glm::vec3 position;
		glm::vec3 rotationAxis;
		glm::vec3 scale;
		float rotationAngle;

		ModelData ReadModelData(std::vector<std::string>& transformDataString)
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
	};

	struct PointLightData
	{
		void ReadPointLightData(std::vector<std::string>& dataString)//, 
			//std::pair<std::string, std::string>(EntityManager::*GenerateKeyValuePair)(std::string, std::string),
			//glm::vec3(EntityManager::*ParseVector)(std::string))
		{
			for (std::string data : dataString)
			{
				if (data == "")
					continue;
				data.erase(std::remove(data.begin(), data.end(), '\t'), data.end());
				data.erase(std::remove(data.begin(), data.end(), ' '), data.end());
				std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(data, ":");
				if (keyValuePair.first == "Ambient")
				{
					ambient = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "Specular")
				{
					specular = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "Diffuse")
				{
					diffuse = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "Position")
				{
					position = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "EulerAngles")
				{
					rotationAxis = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "RotationAngle")
				{
					rotationAngle = strtof(keyValuePair.second.c_str(), nullptr);
				}
				else if (keyValuePair.first == "Scale")
				{
					scale = ParseVector(keyValuePair.second);
				}
				else
				{
					std::cout << "The " << keyValuePair.first << " is not supported in PointLight" << std::endl;
				}
			}
		}

		glm::vec3 ambient;
		glm::vec3 specular;
		glm::vec3 diffuse;
		
		glm::vec3 position;
		glm::vec3 rotationAxis;
		float rotationAngle;
		glm::vec3 scale;
	};
private://TODO: Implement
	//The string is the file path of the entity
	std::unordered_map<std::string, EntityProperties*> entityPropertiesMap;
	std::vector<Entity*> entities;
public:
	std::unordered_map<std::string, Entity*> entitiesByTag;

	void Initialize(ObjectManager* in_objectManager);
	void LoadScene(std::string scenePath);
	EntityProperties* LoadPrefab(std::string prefabPath);
	EntityProperties* LoadProperties(std::string prefabPath);
	//LoadEntitiesFromSceneFile(char* entityName);//Instantiate entities from loaded entity properties
	//Scene file has prefab ids, and transform data;
	void InstantiateEntity(EntityProperties* entityProperties, glm::vec3 startPos, glm::vec3 startEulerAngles, float rotationAngle, glm::vec3 startScale, Entity* parent);

	void Update(float gameTime);
	void Render();

	Entity* GetEntity(int i);

	//ModelData ReadModelData(std::vector<std::string> transformDataString);
	//PointLightData ReadPointLightData(std::vector<std::string> dataString);
};
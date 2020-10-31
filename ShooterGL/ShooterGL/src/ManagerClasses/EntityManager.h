#pragma once
#include <unordered_map>
#include "Entity.h"
#include <vector>
#include "../RigidBody.h"
#include "PhysicsManager.h"
#include "FileReader.h"

class ObjectManager;
class LightManager;

class EntityManager
{
private:
	ObjectManager* objectManager;
public://TODO: Implement

	struct EntityData
	{
		std::string componentName;
		std::vector<std::string> componentProperties;
	};

	struct EntityProperties
	{
		bool isDataLoaded = false;
		//components
		std::vector<EntityData> entityData;
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

		void ReadModelData(std::vector<std::string> transformDataString)
		{

			for (std::string data : transformDataString)
			{
				data.erase(std::remove(data.begin(), data.end(), '\t'), data.end());
				data.erase(std::remove(data.begin(), data.end(), ' '), data.end());
				if (data == "")
					continue;
				std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(data, ":");
				if (keyValuePair.first == "Path")
				{
					modelPath = keyValuePair.second;
				}
				else if (keyValuePair.first == "PositionOffset")
				{
					position = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "EulerAngles")
				{
					rotationAxis = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "RotationAngle")
				{
					rotationAngle = strtof((char*)keyValuePair.second.c_str(), nullptr);
				}
				else if (keyValuePair.first == "ScaleOffset")
				{
					scale = ParseVector(keyValuePair.second);
				}
				else if (keyValuePair.first == "Material")
				{
					materialPath = keyValuePair.second;
				}
			}
		}
	};

private:
	//The string is the file path of the entity
	std::unordered_map<std::string, EntityProperties*> entityPropertiesMap;
	std::vector<Entity*> entities;
public:
	//TODO: Implement. This will be a linked list with a pointer to an entityNode. Each entityNode has a pointer to the next entityNode, and an entity
	std::unordered_map<std::string, Entity*> entitiesByTag;

	void Initialize(ObjectManager* in_objectManager);
	void LoadScene(std::string scenePath);
	EntityProperties* LoadEntityFromFile(std::string prefabPath);
	EntityProperties* LoadProperties(std::string prefabPath);
	//Scene file has prefab ids, and transform data;
	void InstantiateEntity(EntityProperties* entityProperties, glm::vec3 startPos, glm::vec3 angleAxis, float rotationAngle, glm::vec3 startScale, Entity* parent);

	void Update(float gameTime);
	void Render(LightManager* lightManager);

	Entity* GetEntity(int i);
};
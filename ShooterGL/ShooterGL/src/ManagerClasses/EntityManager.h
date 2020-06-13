#pragma once
#include <unordered_map>
#include "Entity.h"
#include <vector>

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
private://TODO: Implement
	//The string is the file path of the entity
	std::unordered_map<std::string, EntityProperties*> entityPropertiesMap;
	std::vector<Entity*> entities;
public:
	std::unordered_map<std::string, Entity*> entitiesByTag;

	void Initialize(ObjectManager* in_objectManager, std::string scenePath);
	glm::vec3 ParseVector(std::string line);
	std::pair<std::string, std::string> GenerateKeyValuePair(std::string line, std::string delimiter);
	EntityProperties* LoadPrefab(std::string prefabPath);
	EntityProperties* LoadProperties(std::string prefabPath);
	//LoadEntitiesFromSceneFile(char* entityName);//Instantiate entities from loaded entity properties
	//Scene file has prefab ids, and transform data;
	void InstantiateEntity(EntityProperties entityProperties, glm::vec3 startPos, glm::vec3 startRotationAxis, float rotationAngle, glm::vec3 startScale, Entity* parent);

	void Update(float gameTime);

	Entity* GetEntity(int i);
};
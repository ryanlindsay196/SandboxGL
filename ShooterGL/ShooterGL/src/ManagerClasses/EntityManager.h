#pragma once
#include <unordered_map>
#include "Entity.h"
#include <vector>

class ObjectManager;

class EntityManager
{
private:
	ObjectManager* objectManager;
public://TODO: Remove
	struct EntityProperties
	{
		//components
		std::vector<std::string> componentNames;
		//tags
		std::vector<std::string> tags;
		////children
		//std::vector<std::string> childEntities;
	};
private://TODO: Remove
	//The string is the name of the entity
	std::unordered_map<std::string, EntityProperties*> entityPropertiesMap;
public:
	std::unordered_map<std::string, Entity*> entitiesByTag;
	std::vector<Entity*> entities;

	void Initialize(ObjectManager* in_objectManager);
	//LoadPropsFromPrefabsFile();
	//LoadEntitiesFromSceneFile(char* entityName);//Instantiate entities from loaded entity properties
	//Scene file has prefab ids, and transform data;
	void InstantiateEntity(EntityProperties entityProperties, glm::vec3 startPos, glm::vec3 startRotationAxis, float rotationAngle, glm::vec3 startScale, Entity* parent);

	void Update(float gameTime);
};
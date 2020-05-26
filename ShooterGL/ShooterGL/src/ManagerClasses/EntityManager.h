#pragma once
#include <unordered_map>
#include "Entity.h"
#include <vector>

class EntityManager
{
	struct EntityProperties
	{
		//components
		std::vector<std::string> componentNames;
		//tags
		std::vector<std::string> tags;
		////children
		//std::vector<std::string> childEntities;
	};

	//The string is the name of the entity
	std::unordered_map<std::string, EntityProperties*> entityPropertiesMap;
public:
	std::unordered_map<std::string, Entity*> entitiesByTag;
	std::vector<Entity*> entities;
	//LoadPropsFromPrefabsFile();
	//LoadEntitiesFromSceneFile(char* entityName);//Instantiate entities from loaded entity properties
	//Scene file has prefab ids, and transform data;

	void Update(float gameTime);
};
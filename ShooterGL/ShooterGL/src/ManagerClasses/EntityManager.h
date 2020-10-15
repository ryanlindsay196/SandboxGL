#pragma once
#include <unordered_map>
#include "Entity.h"
#include <vector>
#include "../RigidBody.h"
#include "PhysicsManager.h"

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
	};

	struct PointLightData
	{
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
	glm::vec3 ParseVector(std::string line);
	std::pair<std::string, std::string> GenerateKeyValuePair(std::string line, std::string delimiter);
	EntityProperties* LoadPrefab(std::string prefabPath);
	EntityProperties* LoadProperties(std::string prefabPath);
	//LoadEntitiesFromSceneFile(char* entityName);//Instantiate entities from loaded entity properties
	//Scene file has prefab ids, and transform data;
	void InstantiateEntity(EntityProperties* entityProperties, glm::vec3 startPos, glm::vec3 startEulerAngles, float rotationAngle, glm::vec3 startScale, Entity* parent);

	void Update(float gameTime);
	void Render();

	Entity* GetEntity(int i);

	ModelData ReadModelData(std::vector<std::string> transformDataString);
	PointLightData ReadPointLightData(std::vector<std::string> dataString);
};
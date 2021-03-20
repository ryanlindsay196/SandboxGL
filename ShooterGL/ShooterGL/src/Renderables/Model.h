#pragma once
#include "../WorldComponent.h"
#include <unordered_map>
#include <memory>

class Animation;
class Shader;
class ObjectManager;
class TextureManager;
class Mesh;
class ModelData;
struct aiMesh;
struct aiNode;
struct aiScene;
class LightManager;

struct Node;
class BoneData;

class Model : public WorldComponent
{
public:
	//std::string directory;

	~Model();

	//Initialize variables and load model vertex data
	void Initialize(glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset, std::string& modelPath, std::string& materialPath);
	//Interface with ASSIMP library to get model data, which are loaded into internal data structures
	void LoadModel(std::string modelPath, std::string materialPath);
	//Process each aiNode from ASSIMP recursively, and copy relevant data to the currentNode variable
	void ProcessNode(aiNode* node, const aiScene* scene, std::string materialPath, Node* currentNode, Node* parentNode);
	//Load vertex data for a new mesh
	Mesh ProcessMesh(aiMesh* mesh, std::string& materialPath, const aiNode* node);
	
	//Set the internal isActive variable, which enables/disables rendering for the model
	void SetIsActive(bool newIsActive);

	//Load a shader for each mesh
	void LoadShaders();

	//Get the number of meshes referenced by this model
	unsigned int GetLoadedMeshesCount();
	//Get a mesh at index i
	//Returns nullptr if i is greater than the size of the meshes vector
	Mesh* GetMesh(unsigned int i);

	//Updates the model and animation
	virtual void Update(float gameTime) override;
	//Renders the model
	void Render(LightManager* lightManager);

private:
	//Manages loaded textures
	TextureManager* m_textureManager;

	//Contains vertices for all meshes. Can be referenced by other model instances
	std::shared_ptr<ModelData> m_modelData;
	//Contains mesh data necessary for each model instance
	std::vector<Mesh> m_meshes;
	//Render only if this is true
	bool isActive = true;

	//Index of the current animation
	unsigned int animationIndex;

	//Time elapsed for the current animation
	float animationTime;

	//The number of bones for the model
	unsigned int totalBones;

	//Used to calculate and send bone data to the shader
	std::unordered_map<std::string, BoneData> boneMap;
};


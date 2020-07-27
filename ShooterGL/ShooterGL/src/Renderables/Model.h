#pragma once
//#include "Component.h"
#include "../WorldComponent.h"
#include <unordered_map>
//#include "assimp/scene.h"

class Animation;
class Shader;
class ObjectManager;
class TextureManager;
class Mesh;
class ModelData;
struct aiMesh;
struct aiNode;
struct aiScene;

struct Node;
class BoneData;

class Model : public WorldComponent
{
public:
	std::string directory;

	Model();
	~Model();

	void Initialize(ObjectManager* objectManager, glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset, char * modelPath, char * materialPath);
	void LoadModel(std::string modelPath, std::string materialPath);
	void ProcessNode(aiNode* node, const aiScene* scene, std::string materialPath, Node* currentNode, Node* parentNode);
	Mesh ProcessMesh(aiMesh* mesh, char* materialPath, const aiNode* node);
	//vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	
	void LoadShaders();

	//Shader* GetShader();
	unsigned int GetLoadedMeshesCount();
	Mesh* GetMesh(unsigned int i);

	virtual void Update(float gameTime) override;
	void Render();

private:
	ObjectManager* m_objectManager;
	TextureManager* m_textureManager;
	ModelData* m_modelData;
	//Shader* shader;
	std::vector<Mesh> m_meshes;
	Node* rootNode;

	std::vector<Animation> animations;
	unsigned int animationIndex;

	std::unordered_map<std::string, BoneData> boneMap;


	//TODO: Delete
	float tempAnimTime = 0;
};


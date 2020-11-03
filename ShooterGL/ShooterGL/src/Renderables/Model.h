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

	Model();
	~Model();

	void Initialize(ObjectManager* objectManager, glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset, std::string& modelPath, std::string& materialPath);
	void LoadModel(std::string modelPath, std::string materialPath);
	void ProcessNode(aiNode* node, const aiScene* scene, std::string materialPath, Node* currentNode, Node* parentNode);
	Mesh ProcessMesh(aiMesh* mesh, std::string& materialPath, const aiNode* node);
	
	void LoadShaders();

	unsigned int GetLoadedMeshesCount();
	Mesh* GetMesh(unsigned int i);

	virtual void Update(float gameTime) override;
	void Render(LightManager* lightManager);

private:
	ObjectManager* m_objectManager;
	TextureManager* m_textureManager;
	std::shared_ptr<ModelData> m_modelData;
	std::vector<Mesh> m_meshes;
	Node* rootNode;

	std::vector<Animation> animations;
	unsigned int animationIndex;

	unsigned int totalBones;

	std::unordered_map<std::string, BoneData> boneMap;
};


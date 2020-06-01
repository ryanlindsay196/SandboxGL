#pragma once
//#include "Component.h"
#include "../WorldComponent.h"
//#include "assimp/scene.h"

class Shader;
class ObjectManager;
class TextureManager;
class Mesh;
struct aiMesh;
struct aiNode;
struct aiScene;

class Model : public WorldComponent
{
public:
	std::string directory;

	Model();
	~Model();

	void Initialize(ObjectManager* objectManager, glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset, char * modelPath, char * materialPath);
	void LoadModel(std::string modelPath);
	void ProcessNode(aiNode* node, const aiScene* scene, std::string materialPath);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, char* materialPath);
	//vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	
	void SetDefaultShaders();
	void SetShaders(char* vertexPath, char* fragmentPath);
	void SetVertexShader(char* vertexPath);
	void SetFragmentShader(char* fragmentPath);

	//Shader* GetShader();
	unsigned int GetLoadedMeshesCount();
	Mesh* GetMesh(int i);

	virtual void Update(float gameTime) override;
	void Render();

private:
	unsigned int VBO;//Vertex buffer object
	unsigned int VAO;//Vertex array object - stores settings from VBO
	unsigned int EBO;//Element buffer object (Index buffer) - stores indices of vertices so we only store each vertex once

	//struct Vertex {
	//	glm::vec3 Position;
	//	glm::vec3 Normal;
	//	glm::vec2 TexCoords;
	//};
	//std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;

	ObjectManager* m_objectManager;
	TextureManager* m_textureManager;
	//Shader* shader;
	std::vector<Mesh> m_meshes;
};


#include "Mesh.h"
//#include "GLFW/glfw3.h"
#include "gtc/matrix_transform.hpp"
#include "glad/glad.h"
#include <string>
#include <algorithm>
#include "ManagerClasses/ObjectManager.h"
#include "ManagerClasses/ShaderManager.h"
#include <assimp/scene.h>
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include "ModelData.h"

#define ARRAYSIZE(a) \
  ((sizeof(a) / sizeof(*(a))) / \
  static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

Mesh::Mesh(ObjectManager * objectManager, const aiScene * aiScene, aiMesh* mesh, char * materialPath, WorldComponent * newParent, MeshData* meshData)
{
	m_materialPath = materialPath;
	//m_modelData = modelData;
	parentMesh = newParent;
	yaw = -90;
	textureManager = objectManager->textureManager;
	m_objectManager = objectManager;
	m_meshData = meshData;

	scaleOffset = glm::mat4(1);
	//TODO: Delete rotationQuat?
	rotationQuat = glm::quat();
	positionOffset = glm::mat4(1);

	offsetTransform = glm::mat4(1);

	if (meshData->vertices.size() > 0)
		return;
	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.Bitangent = vector;

		for (unsigned int k = 0; k < ARRAYSIZE(vertex.WeightValue); k++)
		{
			vertex.WeightValue[k] = 0;
			vertex.BoneID[k] = 0;
		}
		meshData->vertices.push_back(vertex);



	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			meshData->indices.push_back(face.mIndices[j]);
	}

	// process materials
	//aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	//vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	//vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	//std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	//std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	unsigned int numBones = 0;
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int BoneIndex = 0;
		std::string BoneName(mesh->mBones[i]->mName.data);

		if (boneMap.find(BoneName) == boneMap.end())
		{
			BoneIndex = numBones;
			numBones++;
			BoneData bd;
			std::pair<std::string, BoneData> boneMapEntry = std::pair<std::string, BoneData>(BoneName, bd);
			boneMap[BoneName].boneID = BoneIndex;
			boneMap.insert(boneMapEntry);
		}
		else
			BoneIndex = boneMap[BoneName].boneID;

		//TODO: Move this into the if statement above?
		boneMap[BoneName].boneID = BoneIndex;
		boneMap[BoneName].SetTransform(glm::mat4(
			mesh->mBones[i]->mOffsetMatrix.a1, mesh->mBones[i]->mOffsetMatrix.b1, mesh->mBones[i]->mOffsetMatrix.c1, mesh->mBones[i]->mOffsetMatrix.d1,
			mesh->mBones[i]->mOffsetMatrix.a2, mesh->mBones[i]->mOffsetMatrix.b2, mesh->mBones[i]->mOffsetMatrix.c2, mesh->mBones[i]->mOffsetMatrix.d2,
			mesh->mBones[i]->mOffsetMatrix.a3, mesh->mBones[i]->mOffsetMatrix.b3, mesh->mBones[i]->mOffsetMatrix.c3, mesh->mBones[i]->mOffsetMatrix.d3,
			mesh->mBones[i]->mOffsetMatrix.a4, mesh->mBones[i]->mOffsetMatrix.b4, mesh->mBones[i]->mOffsetMatrix.c4, mesh->mBones[i]->mOffsetMatrix.d4
		));
		//boneMap[BoneName].SetTransform(glm::mat4(
		//	mesh->mBones[i]->mOffsetMatrix.a1, mesh->mBones[i]->mOffsetMatrix.a2, mesh->mBones[i]->mOffsetMatrix.a3, mesh->mBones[i]->mOffsetMatrix.a4,
		//	mesh->mBones[i]->mOffsetMatrix.b1, mesh->mBones[i]->mOffsetMatrix.b2, mesh->mBones[i]->mOffsetMatrix.b3, mesh->mBones[i]->mOffsetMatrix.b4,
		//	mesh->mBones[i]->mOffsetMatrix.c1, mesh->mBones[i]->mOffsetMatrix.c2, mesh->mBones[i]->mOffsetMatrix.c3, mesh->mBones[i]->mOffsetMatrix.c4,
		//	mesh->mBones[i]->mOffsetMatrix.d1, mesh->mBones[i]->mOffsetMatrix.d2, mesh->mBones[i]->mOffsetMatrix.d3, mesh->mBones[i]->mOffsetMatrix.d4
		//));

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			//unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
			unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = mesh->mBones[i]->mWeights[j].mWeight;

			for (unsigned int k = 0; k < ARRAYSIZE(meshData->vertices[j].WeightValue); k++)
			{
				if (meshData->vertices[j].WeightValue[k] == 0)
				{
					meshData->vertices[j].WeightValue[k] = Weight;
					meshData->vertices[j].BoneID[k] = BoneIndex;
					//Bones[VertexID].AddBoneData(BoneIndex, Weight);
				}
			}
		}
	}

	SetupMesh();
}

Mesh::Mesh(ObjectManager * objectManager, char * materialPath, WorldComponent * newParent, MeshData * meshData)
{
	m_materialPath = materialPath;
	//m_modelData = modelData;
	parentMesh = newParent;
	yaw = -90;
	textureManager = objectManager->textureManager;
	m_objectManager = objectManager;
	m_meshData = meshData;

	scaleOffset = glm::mat4(1);
	//TODO: Delete rotationQuat?
	rotationQuat = glm::quat();
	positionOffset = glm::mat4(1);

	offsetTransform = glm::mat4(1);
	if (meshData->vertices.size() > 0)
		return;

}

void Mesh::LoadShaders()
{
	if (m_materialPath != "")
		SetShaders((char*)m_materialPath.c_str());
	else
		shader->Initialize(m_objectManager, (char*)"Shaders/VertexDefault.glsl", (char*)"Shaders/FragmentDefault.glsl", (char*)m_materialPath.c_str());
}

void Mesh::SetDefaultShaders()
{
	SetShaders((char*)"Resources/Materials/DefaultMaterial.mat");
}

void Mesh::SetShaders(char* materialPath)
{
	shader = m_objectManager->shaderManager->LoadNewShader(materialPath, m_objectManager);
}

void Mesh::AttachMeshData(MeshData * meshData)
{
	m_meshData = meshData;
}

Shader * Mesh::GetShader()
{
	return shader;
}

//TODO: Change name to Render()
void Mesh::Render()
{
	//if (parentMesh->componentParent != nullptr)
	//	offsetTransform = parentMesh->componentParent->GetTransform();
	//offsetTransform *= positionOffset * glm::toMat4(rotationQuat) * scaleOffset;
	
	//shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform * parentMesh->GetOffsetTransform());
	//shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform);
	//shader->SetShaderUniform_mat4fv((char*)"model", parentMesh->componentParent->GetTransform() * offsetTransform);
	//shader->SetShaderUniform_mat4fv((char*)"model", parentMesh->GetOffsetTransform() * offsetTransform);
	//RotateQuaternion(glm::vec3(1, 1, 1), 0.2f);
	//shader->SetShaderUniform_mat4fv((char*)"model", parentMesh->componentParent->GetTransform());
	//shader->SetShaderUniform_mat4fv((char*)"model", glm::mat4(1));
	unsigned int boneIndex = 0;
	for (auto it : boneMap)
	{
		std::string boneUniform = "gBones[" + std::to_string(boneIndex) + "]";
		shader->SetShaderUniform_mat4fv((char*)boneUniform.c_str(), it.second.GetOffsetTransform());
		shader->SetShaderUniform_mat4fv((char*)boneUniform.c_str(), glm::mat4(1));
		boneIndex++;
	}

	shader->UseShader();
	shader->BindTextures();
	glBindVertexArray(m_meshData->VAO);
	glDrawElements(GL_TRIANGLES, m_meshData->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &m_meshData->VAO);
	glGenBuffers(1, &m_meshData->VBO);
	glGenBuffers(1, &m_meshData->EBO);

	glBindVertexArray(m_meshData->VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_meshData->VBO);
	glBufferData(GL_ARRAY_BUFFER, m_meshData->vertices.size() * sizeof(Vertex), &m_meshData->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshData->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_meshData->indices.size() * sizeof(unsigned int), &m_meshData->indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	//TODO: modify vertex attribute pointers based on material/shader?
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// vertex boneID
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneID));
	// vertex bone weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, WeightValue));

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::Update(float gameTime)
{
	//SetTransform(parentMesh->componentParent->GetTransform());
}

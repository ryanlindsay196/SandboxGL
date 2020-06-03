#include "Model.h"
#include "Shader.h"
#include <glad/glad.h>
#include "ManagerClasses/TextureManager.h"
#include "ManagerClasses/ObjectManager.h"
#include "Mesh.h"
#include "matrix.hpp"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model()
{

}

Model::~Model()
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
		delete(&m_meshes[i]);
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
}

void Model::Initialize(ObjectManager* objectManager, glm::vec3 initialPositionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 initialScaleOffset, char * modelPath, char * materialPath)
{
	//yaw = -90;
	m_textureManager = objectManager->textureManager;
	m_objectManager = objectManager;
#pragma region To REMOVE?


	//float vertices[] = {
	//	//Front
	//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, -1.0f,
	//	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, -1.0f,
	//	//Back
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
	//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
	//	//Left
	//	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,
	//	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f,
	//	//Right
	//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f,
	//	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f,
	//	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f,
	//	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f,
	//	//Bottom
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f,
	//	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, -1.0f, 0.0f,
	//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f,
	//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f,
	//	//Top
	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f
	//};
	//unsigned int indices[] = {  // note that we start from 0!
	//	0 * 1, 2 * 1, 3 * 1,   // first triangle		tr/bl/tl
	//	0 * 1, 2 * 1, 1 * 1    // second triangle
	//};
	//
	//
	//glGenBuffers(1, &VBO);//
	//
	//glGenVertexArrays(1, &VAO);//
	////Initialization code (done once (unless your object frequently changes))
	////a. bind vertex array object
	//glBindVertexArray(VAO);//
	////2. copy our vertices array in a buffer for OpenGL to use
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	////3. then set our vertex attributes pointers
	////glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	////glEnableVertexAttribArray(0);
	////glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	////glEnableVertexAttribArray(1);
	////glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	////glEnableVertexAttribArray(2);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
#pragma endregion
	LoadModel(modelPath);
	if(materialPath == "")
		SetDefaultShaders();
	//TODO: Else, load shaders from material

	//TODO: Remove these
	//shader->AddNewTexture((char*)"Resources/Textures/Resume.PNG");
	//shader->AddNewTexture((char*)"Resources/Textures/Sword_low_Blade_Normal.png");

	offsetTransform = glm::mat4(1);
	positionOffset = glm::mat4(1);
	scaleOffset = glm::mat4(1);
	scaleOffset = glm::scale(scaleOffset, initialScaleOffset);
	rotationQuat = glm::quat(0, 0, 0, 1);
	rotationQuat *= glm::angleAxis(rotationAngle, rotationAxis);
	rotationQuat = glm::rotate(rotationQuat, glm::vec3(M_PI / 2, 0, 0));
	positionOffset = glm::translate(positionOffset, initialPositionOffset);
	//offsetTransform = glm::scale(offsetTransform, initialScaleOffset);
	//offsetTransform = glm::rotate(offsetTransform, rotationAngle, rotationAxis);
	//offsetTransform = glm::translate(offsetTransform, initialPositionOffset);
	//rotationOffset = initialrotationOffset;
	offsetTransform = positionOffset * glm::toMat4(rotationQuat);
	//shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform);
	//shader->SetShaderUniform_mat4fv((char*)"model", glm::mat4(6000.0f));
}

void Model::LoadModel(std::string modelPath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	//check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) //if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	
	//retrieve the directory path of the filepath
	directory = modelPath.substr(0, modelPath.find_last_of('/'));
	
	//process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, "Resources/Materials/DefaultMaterial.mat");
}

void Model::ProcessNode(aiNode * node, const aiScene * scene, std::string materialPath)
{
	//process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//the node object only contains indices to index the actual objects in the scene.
		//the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene, (char*)materialPath.c_str()));
		
		if (aiString("Cube.028") == node->mName)
		{
			m_meshes[m_meshes.size() - 1].Translate(glm::vec3(30, 0, 0));
			m_meshes[m_meshes.size() - 1].Scale(glm::vec3(30, 0, 0));
		}
		//if(mesh->mNumBones > 0)
			//m_meshes[m_meshes.size() - 1].SetTransform(glm::inverse(glm::mat4(mesh->mBones[0]->mOffsetMatrix.a1, mesh->mBones[0]->mOffsetMatrix.a2, mesh->mBones[0]->mOffsetMatrix.a3, mesh->mBones[0]->mOffsetMatrix.a4,
			//	mesh->mBones[0]->mOffsetMatrix.b1, mesh->mBones[0]->mOffsetMatrix.b2, mesh->mBones[0]->mOffsetMatrix.b3, mesh->mBones[0]->mOffsetMatrix.b4,
			//	mesh->mBones[0]->mOffsetMatrix.c1, mesh->mBones[0]->mOffsetMatrix.c2, mesh->mBones[0]->mOffsetMatrix.c3, mesh->mBones[0]->mOffsetMatrix.c4,
			//	mesh->mBones[0]->mOffsetMatrix.d1, mesh->mBones[0]->mOffsetMatrix.d2, mesh->mBones[0]->mOffsetMatrix.d3, mesh->mBones[0]->mOffsetMatrix.d4)));
		//m_meshes[m_meshes.size() - 1].SetTransform(glm::mat4(1));
		//m_meshes[m_meshes.size() - 1].SetTransform(glm::scale(m_meshes[m_meshes.size() - 1].GetOffsetTransform(), glm::vec3(1, 1, 1)));
		//m_meshes[m_meshes.size() - 1].componentParent = componentParent;
		//m_meshes[m_meshes.size() - 1].SetTransform(glm::mat4(node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
		//	node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
		//	node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
		//	node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4));
		//m_meshes[m_meshes.size() - 1].SetTransform(glm::scale(glm::mat4(1), glm::vec3(0.1f, 0.1f, 0.1f)));
	}
	//after we've processed all of the meshes (if any) when recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, (char*)materialPath.c_str());
	}
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene, char* materialPath)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

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
		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
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

	// return a mesh object created from the extracted mesh data
	return Mesh(m_objectManager, vertices, indices, materialPath, this);
}

#pragma region DELETE?

void Model::SetDefaultShaders()
{
	//if (shader != nullptr)
	//	delete(shader);
	//shader = new Shader();
	//shader->Initialize(m_textureManager, (char*)"Shaders/VertexDefault.glsl", (char*)"Shaders/FragmentDefault.glsl");

}

void Model::SetShaders(char * vertexPath, char * fragmentPath)
{
}

void Model::SetVertexShader(char * vertexPath)
{
}

void Model::SetFragmentShader(char * fragmentPath)
{
}
#pragma endregion

unsigned int Model::GetLoadedMeshesCount()
{
	return m_meshes.size();
}

Mesh * Model::GetMesh(int i)
{
	if (i < m_meshes.size())
		return &m_meshes[i];
	return nullptr;
}

//Shader * Model::GetShader()
//{
//	//TODO: Remove this function and all references
//	return nullptr;
//	//return shader;
//}

void Model::Update(float gameTime)
{
	//offsetTransform = glm::mat4(1);
	//offsetTransform = glm::scale(offsetTransform, initialScaleOffset);
	//offsetTransform = glm::rotate(offsetTransform, rotationAngle, rotationAxis);
	//offsetTransform = glm::translate(offsetTransform, positionOffset);
	WorldComponent::Update(gameTime);
}

void Model::Render()
{
	//glm::vec3 tempVec = glm::vec3(0, 1, 0);
	//shader->SetShaderUniform_vec3((char*)"lightPos", tempVec);
	offsetTransform = positionOffset * glm::toMat4(rotationQuat) * scaleOffset;

	//shader->SetShaderUniform_mat4fv((char*)"view", m_objectManager->cameraManager->GetCamera(0)->viewMatrix);
	//shader->SetShaderUniform_mat4fv((char*)"projection", m_objectManager->cameraManager->GetCamera(0)->projectionMatrix);
	//if (componentParent != nullptr)
	//{
	//	shader->SetShaderUniform_mat4fv((char*)"model", componentParent->GetTransform() * offsetTransform);
	//}
	//else
	//{
	//	shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform);
	//}
	////Drawing code (in render loop)
	//glUseProgram(shader->GetShaderProgram());
	
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"view", m_objectManager->cameraManager->GetCamera(0)->viewMatrix);
		m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"projection", m_objectManager->cameraManager->GetCamera(0)->projectionMatrix);
		if (componentParent != nullptr)
		{
			m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"model", componentParent->GetTransform() * offsetTransform * m_meshes[i].GetOffsetTransform());
			//if(i == 0)
			//	m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"model", componentParent->GetTransform() * glm::translate(m_meshes[i].GetOffsetTransform(), glm::vec3(0, 200, 0)));
		}
		else
		{
			m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"model", offsetTransform * m_meshes[i].GetOffsetTransform());
			//m_meshes[i].shader->SetShaderUniform_mat4fv((char*)"model", glm::translate(glm::scale(m_meshes[i].GetOffsetTransform(), glm::vec3(0.02f, 0.02f, 0.02f)), glm::vec3(20, i * 20, 0)));
		}
		m_meshes[i].Draw();
	}
	//glBindVertexArray(VAO);
	////TODO: Comment this out when shaders work on meshes
	//shader->UseShader();
	//shader->BindTextures();
	////TODO: Set the 6 to the amount of indices
	////glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(0);
	////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

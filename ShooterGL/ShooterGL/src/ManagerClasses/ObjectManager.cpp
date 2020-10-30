#include "ObjectManager.h"
#include "EntityManager.h"
#include "GLFW/glfw3.h"
#include "Renderables/Mesh.h"

#include "EntityManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "ControllerManager.h"
#include "LightManager.h"
#include "ShaderManager.h"
#include "PhysicsManager.h"

//TODO: Remove this
#include "../Renderables/ParticleEmitter.h"

#include <iostream>

void ObjectManager::Initialize(GLFWwindow* window)
{
	//lightPos = glm::vec3(2, 1, 10);

	//TODO: Load fixedUpdateTime and other variables (To Be Determined) from a project settings file
	//30 fps = 2 / 60
	//60 fps = 1 / 60
	fixedUpdateMaxTime = 2.f / 60.f;

	entityManager = new EntityManager();
	modelManager = new ModelManager();
	textureManager = new TextureManager();
	cameraManager = new CameraManager();
	controllerManager = new ControllerManager();
	lightManager = new LightManager();
	shaderManager = new ShaderManager();
	physicsManager = new PhysicsManager();
	
	lightManager->Initialize();
	
	controllerManager->Initialize(window);
	entityManager->Initialize(this);
	modelManager->Initialize(this);
	cameraManager->Initialize(this);

	physicsManager->Initialize(glm::vec3(24, 24, 24), glm::vec3(1, 1, 1));
	entityManager->LoadScene("Resources/Scenes/Test2.scene");
	
	modelManager->LoadShaders();
}

void ObjectManager::Update(float gameTime)
{
	fixedUpdateTimer += gameTime;
	//modelManager->UpdateModels(gameTime);
	entityManager->Update(gameTime);
	if (fixedUpdateTimer >= fixedUpdateMaxTime)
	{
		physicsManager->FixedUpdate(fixedUpdateTimer);
		fixedUpdateTimer = 0;
	}
	cameraManager->Update();
}

void ObjectManager::Render()
{
	//entityManager->GetEntity(0)->SetTranslation(lightPos);
	//TODO: make model rotate with parent entity
	//TODO: Make the rotate function and other transformation functions use mat4
	//entityManager->entities[1]->Rotate(glm::vec3(0, 1, 0));
	//lightPos = entityManager->entities[0]->GetTranslation();

	//entityManager->entities[0]->SetTranslation(entityManager->entities[1]->GetTranslation());

	//for (unsigned int i = 0; i < modelManager->LoadedModelsCount(); i++)
	//{
	//	for (unsigned int j = 0; j < modelManager->GetModel(i)->GetLoadedMeshesCount(); j++)
	//	{
	//		modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"light.position", lightManager->GetLight(0)->componentParent->GetTranslationReference());
	//		modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"light.ambient", lightManager->GetLight(0)->GetAmbientReference());
	//		modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"light.diffuse", lightManager->GetLight(0)->GetDiffuseReference());
	//		modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"light.specular", lightManager->GetLight(0)->GetSpecularReference());
	//		modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"viewPos", cameraManager->GetCamera(0)->componentParent->GetTranslationReference());
	//	}
	//}
	//modelManager->RenderModels();
	entityManager->Render(lightManager);
}

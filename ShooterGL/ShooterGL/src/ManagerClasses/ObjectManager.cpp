#include "ObjectManager.h"
#include "EntityManager.h"
#include "GLFW/glfw3.h"
#include "Renderables/Mesh.h"

//TODO: Delete this later
#include "../Renderables/Shader.h"
//TODO: Delete this later
//glm::vec3 lightPos;

void ObjectManager::Initialize(GLFWwindow* window)
{
	//lightPos = glm::vec3(2, 1, 10);

	entityManager = new EntityManager();
	modelManager = new ModelManager();
	textureManager = new TextureManager();
	cameraManager = new CameraManager();
	controllerManager = new ControllerManager();
	lightManager = new LightManager();
	entityManager->Initialize(this);
	modelManager->Initialize(this);
	cameraManager->Initialize(this);
	lightManager->Initialize();
	controllerManager->Initialize(window);
	//TODO: Make models scale with entity scale
	//TODO: Instantiate entities using entity properties loaded from each individual entity file
	entityManager->InstantiateEntity(EntityManager::EntityProperties(), glm::vec3(0, 0, 2), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1, 1, 1), nullptr);
	cameraManager->CreateCamera(entityManager->GetEntity(0));
	entityManager->GetEntity(0)->AddComponent(controllerManager->CreateController(entityManager->GetEntity(0)));
	modelManager->LoadModel(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1.f, 1.f, 1.f));
	
	entityManager->InstantiateEntity(EntityManager::EntityProperties(), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(2, 1, 1), nullptr);
	modelManager->LoadModel(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(.1f, .1f, .1f));
	entityManager->entities[1]->AddComponent(lightManager->AddLight(glm::vec3(0), glm::vec3(1, 0, 0), 0.f, glm::vec3(1)));
	entityManager->entities[1]->AddComponent(modelManager->GetModel(1));
	
	
	//modelManager->LoadModel(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1.2f, 1.2f, 1.2f));
	//entityManager->GetEntity(1)->AddComponent(modelManager->GetModel(0));

	//TODO: Make models that are children of an entity rotate with that entity
	
	entityManager->InstantiateEntity(EntityManager::EntityProperties(), glm::vec3(0, -0.f, 0), glm::vec3(1, 0, 0), 0.0f, glm::vec3(1, 1, 1), nullptr);
	//modelManager->LoadModel(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), 0.0f, glm::vec3(0.f, 0.f, 0.f));
	//entityManager->GetEntity(1)->AddComponent(modelManager->GetModel(1));


	//move model transformations by entity transformations
#pragma region Load floor


	//Makeshift floor
	//modelManager->LoadModel(glm::vec3(-1, -1, -1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(-1, -1, 0), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(0, -1, -1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(0, -1, 0), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(0, -1, 1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(1, -1, -1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(1, -1, 0), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(-1, -1, 2), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(0, -1, 2), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(1, -1, 2), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(-1, -1, 3), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(0, -1, 3), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(1, -1, 3), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(-1, -1, 4), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(0, -1, 4), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(1, -1, 4), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(-1, -1, 5), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(0, -1, 5), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(1, -1, 5), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(-1, -1, 6), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(0, -1, 6), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));
	//modelManager->LoadModel(glm::vec3(1, -1, 6), glm::vec3(0, 0, 1), glm::radians(0.0f), glm::vec3(1.f, 1.f, 1.f));

	//for(unsigned int i = 1; i < modelManager->LoadedModelsCount(); i++)
	//	entityManager->GetEntity(1)->AddComponent(modelManager->GetModel(i));
#pragma endregion


}

void ObjectManager::Update(float gameTime)
{
	//cameraManager->GetCamera(0)->Rotate(glm::vec3(1, 0, 0), 0.02f);
	//cameraManager->GetCamera(0)->Translate(glm::vec3(0, 0, -0.011f));
	//entityManager->GetEntity(0)->Rotate(glm::vec3(1, 0, 0));
	//modelManager->GetModel(0)->r
	//modelManager->GetModel
	//entityManager->GetEntity(1)->Translate(glm::vec3(0.f, 0.f, -0.001f));
	//entityManager->GetEntity(0)->Translate(glm::vec3(0.f, 0.001f, 0.f));

	entityManager->GetEntity(1)->SetTranslation(glm::vec3(2 * (float)sin((float)glfwGetTime()), 0, 2 * (float)cos((float)glfwGetTime())));

	modelManager->UpdateModels(gameTime);
	entityManager->Update(gameTime);
	cameraManager->Update();

	//entityManager->entities[1]->SetEulerAngles(glm::vec3(0, 1, 0) + entityManager->entities[1]->GetEulerAngles());
	//modelManager->GetModel(3)->RotateQuaternion(glm::vec3(0, 1, 0), 0.2f);
	//lightPos += glm::vec3(0, 0, -gameTime);

}

void ObjectManager::Render()
{
	//entityManager->GetEntity(0)->SetTranslation(lightPos);
	//TODO: make model rotate with parent entity
	//TODO: Make the rotate function and other transformation functions use mat4
	//entityManager->entities[1]->Rotate(glm::vec3(0, 1, 0));
	//lightPos = entityManager->entities[0]->GetTranslation();

	//entityManager->entities[0]->SetTranslation(entityManager->entities[1]->GetTranslation());

	for (unsigned int i = 0; i < modelManager->LoadedModelsCount(); i++)
	{
		for (unsigned int j = 0; j < modelManager->GetModel(i)->GetLoadedMeshesCount(); j++)
		{
			modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"light.position", lightManager->GetLight(0)->componentParent->GetTranslationReference());
			modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"light.ambient", lightManager->GetLight(0)->GetAmbientReference());
			modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"light.diffuse", lightManager->GetLight(0)->GetDiffuseReference());
			modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"light.specular", lightManager->GetLight(0)->GetSpecularReference());
			modelManager->GetModel(i)->GetMesh(j)->GetShader()->SetShaderUniform_vec3((char*)"viewPos", cameraManager->GetCamera(0)->componentParent->GetTranslationReference());
		}
	}
	modelManager->RenderModels();
}

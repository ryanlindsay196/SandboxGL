#include "Player.h"
#include "ManagerClasses/ControllerManager.h"
#include "ManagerClasses/EntityManager.h"
#include "../MathHelperFunctions.h"

Player::Player()
{
	playerInfo.currentHealth = 100;
}

void Player::Initialize(ControllerManager* controllerManager, Entity* newParent, EntityManager* newEntityManager)
{
	Controller* latestInstantiatedController = controllerManager->GetController(controllerManager->TotalControllers() - 1);
	entityManager = newEntityManager;
	//If the controller on this entity was added before this component (player) was
	if (latestInstantiatedController != nullptr && latestInstantiatedController->componentParent == newParent)
	{
		SetController(latestInstantiatedController);
	}
	componentParent = newParent;
}

void Player::Update(float gameTime)
{
	if (controller->CurrentActionInput() && controller->ChangedActionInput())
	{
		if (controller->CurrentActionInput() & Controller::ActionPacket::Action1)
		{
			//TODO: If the hitbox is disjointed or a projectile, the hitbox parent is nullptr, otherwise, the hitbox parent is componentParent
			//TODO: Check that scale can be set either in the prefab file or elsewhere
			Entity* newEntity = entityManager->InstantiateEntity(entityManager->LoadProperties("Resources/Prefabs/Missile.prefab"), componentParent->GetTranslation(), glm::vec3(0,1,0), 0.f, glm::vec3(1), nullptr);
			//newEntity->SetRotation(componentParent->GetRotation());
			newEntity->SetEulerAngles(componentParent->GetEulerAngles());
			RigidBody* rb = newEntity->FindRigidBody();
			rb->SetVelocity(componentParent->GetTargetDirection() * glm::vec3(1,-1,1));
			rb->SetVelocity(componentParent->GetDirection() * glm::vec3(1,-1,1));
			rb->SetProperties(RigidBody::Properties::DestroyOnHit | rb->GetProperties());
			rb->SetSpawnedBy(componentParent);
		}
	}
}

void Player::OnCollisionEnter(Entity * entity)
{
	RigidBody* rigidBody = entity->FindRigidBody();
	if (rigidBody != nullptr && rigidBody->GetProperties() & RigidBody::Properties::HitBox)
	{
		rigidBody->SetIsActive(false);
		playerInfo.health -= 20;
		if (playerInfo.health <= 0)
		{
			printf("Match lost\n");
		}
	}
}

void Player::SetController(Controller * newController)
{
	controller = newController;
}

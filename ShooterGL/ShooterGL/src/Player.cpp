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
			//TODO: Find angleAxis function that works
			AngleAxis entityAngleAxis = MathHelperFunctions::EulerToAngleAxis(componentParent->GetEulerAngles());
			//TODO: If the hitbox is disjointed or a projectile, the hitbox parent is nullptr, otherwise, the hitbox parent is componentParent
			Entity* newEntity = entityManager->InstantiateEntity(entityManager->LoadProperties("Resources/Prefabs/Missile.prefab"), componentParent->GetTranslation(), entityAngleAxis.axis, entityAngleAxis.angle, glm::vec3(1), nullptr);
			//newEntity->SetRotation(componentParent->GetRotation());
			newEntity->SetEulerAngles(componentParent->GetEulerAngles());
		}
	}
}

void Player::OnCollisionEnter(Entity * entity)
{
	HitBox* hitBox = entity->FindHitBox();
	if (hitBox != nullptr)
	{
		playerInfo.health -= hitBox->damage;
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

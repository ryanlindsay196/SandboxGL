#include "Player.h"
#include "ManagerClasses/ControllerManager.h"

Player::Player()
{
	playerInfo.currentHealth = 100;
}

void Player::Initialize(ControllerManager* controllerManager, Entity* newParent)
{
	Controller* latestInstantiatedController = controllerManager->GetController(controllerManager->TotalControllers() - 1);

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
		printf("Action input pressed %i\n", controller->CurrentActionInput());
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

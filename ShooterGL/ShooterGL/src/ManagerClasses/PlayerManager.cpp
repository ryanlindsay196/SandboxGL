#include "PlayerManager.h"

PlayerManager* PlayerManager::instance = 0;

PlayerManager::PlayerManager()
{
}

PlayerManager * PlayerManager::GetInstance()
{
	if (!instance)
		instance = new PlayerManager();
	return instance;
}

Player* PlayerManager::AddPlayer(unsigned int newEntityComponentIndex, Entity* parentEntity)
{
	AddComponent<Player>(players, newEntityComponentIndex, parentEntity);
	players[players.size() - 1].Initialize();
	return &players[players.size() - 1];
}

Player * PlayerManager::GetPlayer(int i)
{
	if(i < players.size())
		return &players[i];
	return nullptr;
}

unsigned int PlayerManager::TotalPlayers()
{
	return players.size();
}

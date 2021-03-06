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

void PlayerManager::AddPlayer(Player * newPlayer)
{
	players.push_back(newPlayer);
}

Player * PlayerManager::GetPlayer(int i)
{
	if(i < players.size())
		return players[i];
	return nullptr;
}

unsigned int PlayerManager::TotalPlayers()
{
	return players.size();
}

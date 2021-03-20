#pragma once
#include <vector>
#include "BaseManager.h"
#include "Player.h"

class PlayerManager : BaseManager
{
private:
	static PlayerManager* instance;
	PlayerManager();
public:
	static PlayerManager* GetInstance();

	Player* AddPlayer(unsigned int newEntityComponentIndex, Entity* parentEntity);
	Player* GetPlayer(int i);
	unsigned int TotalPlayers();
private:
	std::vector<Player> players;
};
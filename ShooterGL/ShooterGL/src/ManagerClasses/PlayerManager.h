#pragma once
#include <vector>
#include "BaseManager.h"

class Player;

class PlayerManager : BaseManager
{
private:
	static PlayerManager* instance;
	PlayerManager();
public:
	static PlayerManager* GetInstance();

	void AddPlayer(Player* newPlayer);
	Player* GetPlayer(int i);
	unsigned int TotalPlayers();
private:
	std::vector<Player*> players;
};
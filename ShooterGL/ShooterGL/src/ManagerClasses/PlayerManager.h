#pragma once
#include <vector>

class Player;

class PlayerManager
{
public:
	void AddPlayer(Player* newPlayer);
	Player* GetPlayer(int i);
	unsigned int TotalPlayers();
private:
	std::vector<Player*> players;
};
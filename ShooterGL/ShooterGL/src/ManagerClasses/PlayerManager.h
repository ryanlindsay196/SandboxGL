#pragma once
#include <vector>

class Player;

class PlayerManager
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
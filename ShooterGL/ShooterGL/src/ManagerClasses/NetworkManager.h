#pragma once
#include "enet/enet.h"
#include <vector>

class EntityManager;
class Entity;
class Controller;

class NetworkManager
{
public:
	void Initialize(EntityManager* in_entityManager);
	void DeInitialize();
	void Update(float gameTime);
private:
	Entity* InstantiateNetworkedPlayer();
private:
	ENetHost* client;
	//Address and port of the server we're connecting to
	ENetAddress address;
	//Holds all of the events that we're receiving from the server
	ENetEvent enetEvent;
	//The server that we're connecting to
	ENetPeer* peer;

	unsigned int lobbyID;
	unsigned int playerID;

	EntityManager* entityManager;
	std::vector<Controller*> networkedControllers;
};
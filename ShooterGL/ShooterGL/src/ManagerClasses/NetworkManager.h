#pragma once
#include "enet/enet.h"

class EntityManager;

class NetworkManager
{
public:
	void Initialize(EntityManager* in_entityManager);
	void DeInitialize();
	void Update(float gameTime);
private:
	void InstantiateNetworkedPlayer();
private:
	ENetHost* client;
	//Address and port of the server we're connecting to
	ENetAddress address;
	//Holds all of the events that we're receiving from the server
	ENetEvent enetEvent;
	//The server that we're connecting to
	ENetPeer* peer;

	EntityManager* entityManager;
};
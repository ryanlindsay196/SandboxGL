#pragma once
#include "enet/enet.h"
#include <vector>

class EntityManager;
class Entity;
class Controller;
class ControllerManager;

class NetworkManager
{
public:
	void Initialize(EntityManager* in_entityManager, ControllerManager* in_controllerManager);
	void DeInitialize();
	void Update(float gameTime);
private:
	Entity* InstantiateNetworkedPlayer();
	bool SendPacket(std::string* packetData);
	std::vector<std::string> ParsePacket(std::string * packet);
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

	float requestPositionsTimer;
	const float requestPositionsMaxTime = 5;

	EntityManager* entityManager;
	ControllerManager* controllerManager;
};
#pragma once
#include "enet/enet.h"
#include <vector>
#include "glm.hpp"

class EntityManager;
class Entity;
class Controller;
class ControllerManager;

class NetworkManager
{
public:
	//Initialize the server
	void Initialize(EntityManager* in_entityManager, ControllerManager* in_controllerManager);
	//Disconnect from the server
	void DisconnectFromServer();
	//Reads and responds to incoming packets. Sends outgoing packets.
	void Update(float gameTime);
private:
	//Instantiate a player
	Entity* InstantiateNetworkedPlayer();
	//Sends a packet to our connected peer
	bool SendPacket(std::string* packetData);
	//Parses a packet into a series of strings
	std::vector<std::string> ParsePacket(std::string * packet);
	//Connect to our server
	void ConnectToServer();
private:
	ENetHost* client;
	//Address and port of the server we're connecting to
	ENetAddress address;
	//Holds all of the events that we're receiving from the server
	ENetEvent enetEvent;
	//The server that we're connecting to
	ENetPeer* peer;

	//The lobbyID sent to us by the server
	unsigned int lobbyID;
	//The index of our player in the lobby array on the server
	unsigned int playerID;

	///Timers
	float requestPositionsTimer;
	//When the requestPositionsTimer runs out, we set it to this variable
	const float requestPositionsMaxTime = 5.f;

	//The value of our local player's euler angles, when we last sent them.
	glm::vec3 latestEulerAnglesSent;

	EntityManager* entityManager;
	ControllerManager* controllerManager;
};
#pragma once
#include "enet/enet.h"
class NetworkManager
{
public:
	void Initialize();
	void DeInitialize();
	void Update(float gameTime);

private:

	ENetHost* client;

	//Address and port of the server we're connecting to
	ENetAddress address;
	//Holds all of the events that we're receiving from the server
	ENetEvent enetEvent;
	//The server that we're connecting to
	ENetPeer* peer;
};
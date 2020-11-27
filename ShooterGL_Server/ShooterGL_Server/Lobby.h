#pragma once
#include "enet/enet.h"
#include <vector>

struct PeerData {
	ENetPeer* peer;
	float timeOutTimer;
};

//struct PacketData
//{
//	ENetPacket* packet;
//	float deleteTimer;
//};

const int timeOutMaxTime = 100000;

class Lobby {
public:
	void Update(float gameTime);

	unsigned int AddPeer(ENetPeer& peer);
	void RemovePeer(ENetPeer& peer);
	void RemovePeer(unsigned int peerID);

	bool SendPacket(unsigned int peerID, std::string * packetData);

	//Sends a packet to all peers in the lobby
	bool BroadcastPacket(std::string * packetData);
	bool BroadcastPacket(std::string * packetData, int playerIDToExclude);
private:
	PeerData peers[4];

	void RequestPositions();
};
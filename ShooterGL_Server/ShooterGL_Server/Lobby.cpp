#include "Lobby.h"

void Lobby::Update(float gameTime)
{
	//Check if any peer connections have timed out
	for (int i = 0; i < sizeof(peers) / sizeof(PeerData); i++)
	{
		peers[i].timeOutTimer -= gameTime;
		//If the peer exists and has timed out
		if (peers[i].peer != nullptr && peers[i].timeOutTimer <= 0)
			RemovePeer(*peers[i].peer);
	}
}

unsigned int Lobby::AddPeer(ENetPeer & peer)
{
	for (unsigned int i = 0; i < sizeof(peers) / sizeof(PeerData); i++)
	{
		if (peers[i].peer == nullptr)
		{
			peers[i].peer = &peer;
			peers[i].timeOutTimer = timeOutMaxTime;
			return i;
		}
	}
}

void Lobby::RemovePeer(ENetPeer & peer)
{
	for (int i = 0; i < sizeof(peers) / sizeof(PeerData); i++)
	{
		if (peers[i].peer == &peer)
		{
			peers[i].peer = nullptr;
		}
	}
}

void Lobby::RemovePeer(unsigned int peerID)
{
	if (peerID < sizeof(peers))
	{
		peers[peerID] = PeerData();
	}
}

//Returns true if a packet is sent
bool Lobby::SendPacket(unsigned int peerID, std::string * packetData)
{
	//If looking for a peer that is out of bounds
	if (peerID > sizeof(peers))
		return false;
	//If there isn't a peer in the current slot
	if (peers[peerID].peer == nullptr)
		return false;
	ENetPacket* packet = enet_packet_create(packetData->c_str(), strlen(packetData->c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peers[peerID].peer, 0, packet);

	return true;
}

bool Lobby::BroadcastPacket(std::string * packetData)
{
	ENetPacket* packet = enet_packet_create(packetData->c_str(), strlen(packetData->c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	for (int i = 0; i < sizeof(peers) / sizeof(PeerData); i++)
	{
		//If there isn't a peer in the current slot
		if (peers[i].peer != nullptr)
		{
			enet_peer_send(peers[i].peer, 0, packet);
		}
	}
	return true;
}

bool Lobby::BroadcastPacket(std::string * packetData, int playerIDToExclude)
{
	ENetPacket* packet = enet_packet_create(packetData->c_str(), strlen(packetData->c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	for (int i = 0; i < sizeof(peers) / sizeof(PeerData); i++)
	{
		if (i == playerIDToExclude)
			continue;
		//If there is a peer in the current slot
		if (peers[i].peer != nullptr)
		{
			enet_peer_send(peers[i].peer, 0, packet);
		}
	}
	return true;
}

void Lobby::RequestPositions()
{
	std::string packetData = "RequestPositions";
	BroadcastPacket(&packetData);
}

#include <iostream>
#include <enet/enet.h>
#include <vector>
#include <string>
#include "Lobby.h"
#pragma warning (disable : 4996)

std::vector<std::string> ParsePacket(ENetPacket* packet);

int main()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

	ENetAddress address;
	ENetHost* server;
	ENetEvent enetEvent;

	address.host = ENET_HOST_ANY;
	address.port = 69;

	server = enet_host_create(&address, 32, 1, 0, 0);

	if (server == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host!\n");
		return EXIT_FAILURE;
	}

	//Server game loop start
	Lobby lobby = Lobby();
	while (true)
	{
		lobby.Update(0.01f);

		while (enet_host_service(server, &enetEvent, 0) > 0)
		{
			std::vector<std::string> packetStrings;
			switch (enetEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n",
					enetEvent.peer->address.host,
					enetEvent.peer->address.port);
				{
					unsigned int newPlayerID = lobby.AddPeer(*enetEvent.peer);


					std::string packetData = "InitPlayerID:" + std::to_string(newPlayerID);
					lobby.SendPacket(newPlayerID, &packetData);
					printf(packetData.c_str()); printf("\n");
					packetData = "RequestPositions:" + std::to_string(newPlayerID);
					lobby.BroadcastPacket(&packetData, newPlayerID);
					//lobby.SendPacket(newPlayerID, &packetData);
					
					
					//packetData = "WASD:0:1";
					//lobby.BroadcastPacket(&packetData);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				//printf("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
				//	enetEvent.packet->dataLength,
				//	enetEvent.packet->data,
				//	enetEvent.peer->address.host,
				//	enetEvent.peer->address.port,
				//	enetEvent.channelID);

				packetStrings = ParsePacket(enetEvent.packet);

				if (packetStrings.size() > 0)
				{
					if (packetStrings[0] == "PositionData")
					{
						///[0] = Header ("PositionData")
						///[1] = LobbyID
						///[2] = PlayerID of sender
						///[3] = Position
						///[4] = Rotation
						///[5] = Scale
						///[6] = PlayerID of receiver

						std::string packetData = (char*)enetEvent.packet->data;
						//TODO: When lobby is made into a list, broadcast packet to lobby[stoi(packetStrings[1])]
						//lobby.BroadcastPacket(&packetData, stoi(packetStrings[2]));
						lobby.SendPacket(stoi(packetStrings[6]), &packetData);
					}
					else if (packetStrings[0] == "RequestPositions")
					{
						std::string packetData = "RequestPositions:" + packetStrings[1];
						//lobby.SendPacket(stoi(packetStrings[1]), &packetData);
						lobby.BroadcastPacket(&packetData, stoi(packetStrings[1]));
					}
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%x:%u disconnected.\n",
					enetEvent.peer->address.host,
					enetEvent.peer->address.port);
				//enetEvent.peer->data = NULL;
				lobby.RemovePeer(enetEvent.peer);
				break;
			}
		}
	}
	//server game loop end
	enet_host_destroy(server);

	return EXIT_SUCCESS;
}

std::vector<std::string> ParsePacket(ENetPacket * packet)
{
	std::vector<std::string> returnStrings;
	std::pair<std::string, std::string> packetPair;

	packetPair.second = std::string((char*)(packet->data)).substr(std::string((char*)(packet->data)).find_first_of(":") + 1);
	packetPair.first = strtok((char*)std::string((char*)packet->data).c_str(), ":");

	while (packetPair.first != packetPair.second)
	{
		returnStrings.push_back(packetPair.first);
		packetPair.first = strtok((char*)std::string((char*)packetPair.second.c_str()).c_str(), ":");
		packetPair.second = std::string((char*)packetPair.second.c_str()).substr(std::string((char*)packetPair.second.c_str()).find_first_of(":") + 1);
	}
	returnStrings.push_back(packetPair.second);
	return returnStrings;
}

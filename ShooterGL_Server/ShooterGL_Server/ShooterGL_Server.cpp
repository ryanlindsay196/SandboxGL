// ShooterGL_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <enet/enet.h>
#include <vector>

std::vector<ENetPeer> peers;

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
	while (true)
	{
		ENetPacket* packet = nullptr;// = enet_packet_create("SpawnPlayer", strlen("SpawnPlayer") + 1, ENET_PACKET_FLAG_RELIABLE);

		while (enet_host_service(server, &enetEvent, 0) > 0)
		{
			switch (enetEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n",
					enetEvent.peer->address.host,
					enetEvent.peer->address.port);
				{
					packet = enet_packet_create("SpawnPlayer", strlen("SpawnPlayer") + 1, ENET_PACKET_FLAG_RELIABLE);

					if (enet_peer_send(enetEvent.peer, 0, packet) < 0)
						printf("Can't send packet");
					//peers.push_back(*enetEvent.peer);


					packet = enet_packet_create("WASD:0:6", strlen("WASD:0:1") + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(enetEvent.peer, 0, packet);
					//enet_packet_destroy(packet);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				printf("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
					enetEvent.packet->dataLength,
					enetEvent.packet->data,
					enetEvent.peer->address.host,
					enetEvent.peer->address.port,
					enetEvent.channelID);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%x:%u disconnected.\n",
					enetEvent.peer->address.host,
					enetEvent.peer->address.port);
				//enetEvent.peer->data = NULL;
				break;
			}
		}
		//enet_packet_destroy(packet);

		//if (enetEvent.peer != NULL)
		//{
		//	packet = enet_packet_create("WASD:0:1", strlen("WASD:0:1") + 1, ENET_PACKET_FLAG_RELIABLE);
		//	enet_peer_send(enetEvent.peer, 3000, packet);
		//	enet_packet_destroy(packet);
		//}
		//for (ENetPeer peer : peers)
		//{
		//	if (&peer != nullptr)
		//	{
		//		packet = enet_packet_create("WASD:0:1", strlen("WASD:0:1") + 1, ENET_PACKET_FLAG_RELIABLE);
		//		enet_peer_send(&peer, 300, packet);
		//		enet_packet_destroy(packet);
		//	}
		//}
	}
	//server game loop end
	enet_host_destroy(server);

	return EXIT_SUCCESS;
}

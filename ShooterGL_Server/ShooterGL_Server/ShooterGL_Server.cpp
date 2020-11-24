// ShooterGL_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <enet/enet.h>

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
		while (enet_host_service(server, &enetEvent, 1000) > 0)
		{
			switch (enetEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n",
					enetEvent.peer->address.host,
					enetEvent.peer->address.port);
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
	}
	//server game loop end

	enet_host_destroy(server);

	return EXIT_SUCCESS;
}

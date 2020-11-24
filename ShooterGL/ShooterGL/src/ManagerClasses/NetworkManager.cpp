#include "NetworkManager.h"
//#include <enet/enet.h>
#include <iostream>
//TODO: Delete?
#include <assert.h>

void NetworkManager::Initialize()
{
	if (enet_initialize() != 0)
	{
		std::cout << stderr << "An error occurred while initializing ENet!" << std::endl;
		assert(0);
	}
	atexit(enet_deinitialize);

	client = enet_host_create(NULL, 1, 1, 0, 0);

	if (client == nullptr)
	{
		std::cout << stderr << "An error occurred while trying to create an ENet client host!" << std::endl;
		assert(0);
	}

	enet_address_set_host(&address, "127.0.0.1");
	address.port = 69;

	peer = enet_host_connect(client, &address, 1, 0);
	if (peer == NULL)
	{
		std::cout << stderr << "No available peers for initiating ENEt connection!" << std::endl;
		assert(0);
	}

	if (enet_host_service(client, &enetEvent, 5000) > 0 &&
		enetEvent.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to 127.0.0.1:69 succeeded!");
	}
	else
	{
		enet_peer_reset(peer);
		puts("Connection to 127.0.0.1:69 failed.");
	}
}

void NetworkManager::DeInitialize()
{
	enet_peer_disconnect(peer, 0);

	while (enet_host_service(client, &enetEvent, 3000) > 0)
	{
		switch (enetEvent.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(enetEvent.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				puts("Disconnection succeeded.");
				break;
		}
	}
}

void NetworkManager::Update(float gameTime)
{
	while (enet_host_service(client, &enetEvent, 1000) > 0)
	{
		switch (enetEvent.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
				enetEvent.packet->dataLength,
				enetEvent.packet->data,
				enetEvent.peer->address.host,
				enetEvent.peer->address.port,
				enetEvent.channelID);
			break;
		}
	}
	ENetPacket* packet = enet_packet_create("packetfoo", strlen("packetfoo") + 1, ENET_PACKET_FLAG_RELIABLE);
	//enet_packet_resize(packet, strlen("packetfoo") + 1);
	//strcpy(&packet->data[strlen("packet")], "foo");
	enet_peer_send(peer, 0, packet);
}

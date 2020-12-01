#include "NetworkManager.h"
//#include <enet/enet.h>
#include <iostream>
//TODO: Delete?
#include <assert.h>

#include "glm.hpp"
#include "EntityManager.h"
#include "ControllerManager.h"
#include "Controller.h"
#include "Entity.h"
#include "FileReader.h"
#include "../MathHelperFunctions.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#pragma warning (disable : 4996)

void NetworkManager::Initialize(EntityManager* in_entityManager, ControllerManager* in_controllerManager)
{
	entityManager = in_entityManager;
	controllerManager = in_controllerManager;

	ConnectToServer();
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
#pragma region Timers
	requestPositionsTimer -= gameTime;
	if (requestPositionsTimer <= 0)
	{
		requestPositionsTimer = requestPositionsMaxTime;
		std::string packetData = "RequestPositions:" + std::to_string(playerID);
		SendPacket(&packetData);
		//std::cout << std::endl << std::endl << "PlayerID = " << playerID << std::endl << std::endl;
	}

	//The angle the local player has changed it's rotation by since last sending rotation to the server
	float rotationDot = dot(MathHelperFunctions::EulerAnglesToQuaternion(latestEulerAnglesSent), 
		MathHelperFunctions::EulerAnglesToQuaternion(controllerManager->GetController(0)->componentParent->GetEulerAngles()));
	if(rotationDot < 0.99999f)
	{
		glm::vec3 eulers = controllerManager->GetController(0)->componentParent->GetEulerAngles();
		std::string packetData = "Rotation:" +
			std::to_string(playerID) + std::string(":") +
			std::to_string(eulers.x) + std::string(",") +
			std::to_string(eulers.y) + std::string(",") +
			std::to_string(eulers.z);
		latestEulerAnglesSent = eulers;
		SendPacket(&packetData);
	}
#pragma endregion

	//Check local player for change in WASD (move direction)
	if (controllerManager->GetController(0)->ChangedWASD() != 0)
	{
		std::string packetData = std::string("WASD:" + std::to_string(playerID) + ":" + std::to_string(controllerManager->GetController(0)->CurrentWASD()));
		SendPacket(&packetData);
	}
	//Check local player for change in Action Input(attacks and stuff)
	if (controllerManager->GetController(0)->ChangedActionInput() != 0)
	{
		std::string packetData = std::string("Action:" + std::to_string(playerID) + ":" + std::to_string(controllerManager->GetController(0)->CurrentActionInput()));
		SendPacket(&packetData);
	}

	while (enet_host_service(client, &enetEvent, 0) > 0)
	{
		switch (enetEvent.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
		{
			std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair((char*)enetEvent.packet->data, ":");
			//printf("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
			//	enetEvent.packet->dataLength,
			//	enetEvent.packet->data,
			//	enetEvent.peer->address.host,
			//	enetEvent.peer->address.port,
			//	enetEvent.channelID);
			printf("%s\n", enetEvent.packet->data);
			if (keyValuePair.first == "SpawnPlayer")
			{
				Entity* newNetworkedPlayer = InstantiateNetworkedPlayer();
				Controller* newNetworkedController = controllerManager->GetController(controllerManager->TotalControllers() - 1);//newNetworkedPlayer->FindController();
				newNetworkedController->SetPlayerID((unsigned int)std::stoi(keyValuePair.second));
				newNetworkedController->SetIsNetworked(true);
			}
			else if (keyValuePair.first == "Rotation")
			{
				///When receiving this, update the rotation of a networked player
				///[0] = Sending player ID (Update the player with this ID on the local machine)
				///[1] = Euler Angles of player to update
				std::vector<std::string> packetStrings = ParsePacket(&keyValuePair.second);
				for (int i = 0; i < controllerManager->TotalControllers(); i++)
				{
					//If we've found the correct networked player on the local machine...
					if (controllerManager->GetController(i)->GetPlayerID() == stoi(packetStrings[0]))
					{
						//Update the networked player's rotation
						controllerManager->GetController(i)->componentParent->SetNetworkedEulerAngles(ParseVector(packetStrings[1]));
						break;
					}
				}
			}
			else if (keyValuePair.first == "WASD")
			{
				std::pair<std::string, std::string> idWASDPair = GenerateKeyValuePair(keyValuePair.second, ":");
				int controllerID = stoi(idWASDPair.first);
				int wasd = stoi(idWASDPair.second);
				for (int i = 0; i < controllerManager->TotalControllers(); i++)
				{
					//Once the correct controller is found, update it's WASD input
					if (controllerManager->GetController(i)->GetIsNetworked() &&
						controllerManager->GetController(i)->GetPlayerID() == controllerID)
					{
						controllerManager->GetController(i)->SetNetworkWASDInput(wasd);
						break;
					}
				}
			}
			else if (keyValuePair.first == "Action")
			{
				std::pair<std::string, std::string> idActionPair = GenerateKeyValuePair(keyValuePair.second, ":");
				int controllerID = stoi(idActionPair.first);
				int action = stoi(idActionPair.second);
				for (int i = 0; i < controllerManager->TotalControllers(); i++)
				{
					//Once the correct controller is found, update it's action input
					if (controllerManager->GetController(i)->GetIsNetworked() &&
						controllerManager->GetController(i)->GetPlayerID() == controllerID)
					{
						controllerManager->GetController(i)->SetNetworkActionInput(action);
						break;
					}
				}
			}
			else if (keyValuePair.first == "RequestPositions")
			{
				Entity* localPlayer = controllerManager->GetController(0)->componentParent;
#pragma region RequestPositions Handler
				//std::string sendingPlayerIDString = std::to_string(playerID);
				std::string sendingPlayerIDString = std::to_string(controllerManager->GetController(0)->GetPlayerID());

				glm::vec3 pos = localPlayer->GetTranslation();
				std::string playerPosition = std::to_string(pos.x) + 
					std::string(",") + 
					std::to_string(pos.y) + 
					std::string(",") + 
					std::to_string(pos.z);
				//glm::quat rotation = localPlayer->GetRotation();
				//std::string playerRotation = std::to_string(rotation.x) + 
				//	std::string(",") + 
				//	std::to_string(rotation.y) + 
				//	std::string(",") + 
				//	std::to_string(rotation.z) + 
				//	std::string(",") + 
				//	std::to_string(rotation.w);
				glm::vec3 eulers = localPlayer->GetEulerAngles();
				std::string playerEulers = std::to_string(eulers.x) +
					std::string(",") +
					std::to_string(eulers.y) +
					std::string(",") +
					std::to_string(eulers.z);

				glm::vec3 scale = localPlayer->GetScale();
				std::string playerScale = std::to_string(scale.x) +
					std::string(",") +
					std::to_string(scale.y) +
					std::string(",") +
					std::to_string(scale.z);
				
				std::string receivingPlayerIDString = keyValuePair.second;

				std::string packetData = "PositionData:" +
					std::to_string(lobbyID) + 
					":" +
					sendingPlayerIDString +
					":" +
					playerPosition +
					":" +
					playerEulers +
					//playerRotation +
					":" +
					playerScale + 
					":" + 
					receivingPlayerIDString;
#pragma endregion

				SendPacket(&packetData);
			}
			else if (keyValuePair.first == "PositionData")
			{
				std::vector<std::string> packetStrings = ParsePacket(&keyValuePair.second);
				///[0] = LobbyID
				///[1] = PlayerID of sender
				///[2] = Position
				///[3] = Rotation
				///[4] = Scale
				///[5] = PlayerID of receiver


				bool playerFound = false;
				for (int i = 0; i < controllerManager->TotalControllers(); i++)
				{
					if (controllerManager->GetController(i)->GetPlayerID() == stoi(packetStrings[1]))
					{
						std::cout << " Updated PLayer's position " << packetStrings[1] << std::endl;
						Entity* playerToUpdate = controllerManager->GetController(i)->componentParent;
						playerToUpdate->SetNetworkedPosition(ParseVector(packetStrings[2]));
						
						playerToUpdate->SetNetworkedEulerAngles(ParseVector(packetStrings[3]));
						
						playerToUpdate->SetScale(ParseVector(packetStrings[4]));
						playerFound = true;
					}
				}
				//If the player we're receiving position data for hasn't been spawned.
				if (!playerFound)
				{
					if (controllerManager->TotalControllers() < 4)
					{
						std::cout << "Spawned player " << keyValuePair.second;
						Entity* newNetworkedPlayer = InstantiateNetworkedPlayer();
						Controller* newNetworkedController = controllerManager->GetController(controllerManager->TotalControllers() - 1);//newNetworkedPlayer->FindController();
						newNetworkedController->SetPlayerID((unsigned int)std::stoi(packetStrings[1]));
						newNetworkedController->SetIsNetworked(true);

						newNetworkedPlayer->SetPosition(ParseVector(packetStrings[2]));

						newNetworkedPlayer->SetEulerAngles(ParseVector(packetStrings[3]));

						newNetworkedPlayer->SetScale(ParseVector(packetStrings[4]));
					}
				}
			}
			else if (keyValuePair.first == "InitPlayerID")
			{
				playerID = stoi(keyValuePair.second);
				controllerManager->GetController(0)->SetPlayerID(playerID);
			}
		}
		break;
		case ENET_EVENT_TYPE_DISCONNECT:
			printf("Disconneted from server.");
			ConnectToServer();
			break;
		}
	}
	//ENetPacket* packet = enet_packet_create("packetfoo", strlen("packetfoo") + 1, ENET_PACKET_FLAG_RELIABLE);
	//enet_packet_resize(packet, strlen("packetfoo") + 1);
	//strcpy(&packet->data[strlen("packet")], "foo");
	//enet_peer_send(peer, 0, packet);
}

Entity* NetworkManager::InstantiateNetworkedPlayer()
{
	return entityManager->InstantiateEntity(entityManager->LoadEntityFromFile("Resources/Prefabs/Player.prefab"), glm::vec3(), glm::vec3(0, 1, 0), 0.f, glm::vec3(1), nullptr);
}

bool NetworkManager::SendPacket(std::string * packetData)
{//Returns true if a packet is sent
	if (peer == nullptr)
		return false;
	ENetPacket* packet = enet_packet_create(packetData->c_str(), strlen(packetData->c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);

	return true;
}

std::vector<std::string> NetworkManager::ParsePacket(std::string * packet)
{
		std::vector<std::string> returnStrings;
		std::pair<std::string, std::string> packetPair;

		packetPair.second = std::string((char*)(packet->c_str())).substr(std::string((char*)(packet->c_str())).find_first_of(":") + 1);
		packetPair.first = strtok((char*)std::string((char*)packet->c_str()).c_str(), ":");

		while (packetPair.first != packetPair.second)
		{
			returnStrings.push_back(packetPair.first);
			packetPair.first = strtok((char*)std::string((char*)packetPair.second.c_str()).c_str(), ":");
			packetPair.second = std::string((char*)packetPair.second.c_str()).substr(std::string((char*)packetPair.second.c_str()).find_first_of(":") + 1);
		}
		returnStrings.push_back(packetPair.second);
		return returnStrings;
}

void NetworkManager::ConnectToServer()
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

	if (enet_host_service(client, &enetEvent, 1000) > 0 &&
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

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

//Initialize the server
void NetworkManager::Initialize(EntityManager* in_entityManager, ControllerManager* in_controllerManager)
{
	entityManager = in_entityManager;
	controllerManager = in_controllerManager;

	ConnectToServer();
}

//Disconnect from the server
void NetworkManager::DisconnectFromServer()
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

//Reads and responds to incoming packets. Sends outgoing packets.
void NetworkManager::Update(float gameTime)
{
#pragma region Timers
	//Once enough time has elapsed, ask for other players' positions
	requestPositionsTimer -= gameTime;
	if (requestPositionsTimer <= 0)
	{
		requestPositionsTimer = requestPositionsMaxTime;
		std::string packetData = "RequestPositions:" + std::to_string(playerID);
		SendPacket(&packetData);
	}
#pragma endregion

	//The angle the local player has changed it's rotation by since last sending rotation to the server
	float rotationDot = dot(MathHelperFunctions::EulerAnglesToQuaternion(latestEulerAnglesSent), 
		MathHelperFunctions::EulerAnglesToQuaternion(controllerManager->GetController(0)->componentParent->GetEulerAngles()));

	//If the local player has rotated above a certain threshold, send a new packet
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

	//Processes all packets received from the server
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
				//Spawns a player using the entityManager and initializes relevant data.
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
					//If found the correct networked player is found on the local machine...
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
				//If we've received keyboard input for a player from the server
				std::pair<std::string, std::string> idWASDPair = GenerateKeyValuePair(keyValuePair.second, ":");
				int controllerID = stoi(idWASDPair.first);
				int wasd = stoi(idWASDPair.second);
				for (int i = 0; i < controllerManager->TotalControllers(); i++)
				{
					//Once the correct controller is found, update it's WASD input
					if (controllerManager->GetController(i)->GetIsNetworked() &&
						controllerManager->GetController(i)->GetPlayerID() == controllerID)
					{
						//Set the input for our networked player
						controllerManager->GetController(i)->SetNetworkWASDInput(wasd);
						break;
					}
				}
			}
			else if (keyValuePair.first == "Action")
			{
				//Similar to WASD, this determines whether a networked player is pressing a button
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
				//Sends the position of the local player
				//[0] Is the header
				//[1] Is the player to send the local player's positions to
				Entity* localPlayer = controllerManager->GetController(0)->componentParent;
#pragma region RequestPositions Handler
				std::string sendingPlayerIDString = std::to_string(controllerManager->GetController(0)->GetPlayerID());

				glm::vec3 pos = localPlayer->GetTranslation();
				std::string playerPosition = std::to_string(pos.x) + 
					std::string(",") + 
					std::to_string(pos.y) + 
					std::string(",") + 
					std::to_string(pos.z);
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
				//Find the (networked) player corresponding to the received position data
				for (int i = 0; i < controllerManager->TotalControllers(); i++)
				{
					if (controllerManager->GetController(i)->GetPlayerID() == stoi(packetStrings[1]))
					{
						//Update the transform of a networked player
						std::cout << " Updated Player's position " << packetStrings[1] << std::endl;
						Entity* playerToUpdate = controllerManager->GetController(i)->componentParent;
						//Set networked position, which the networked player will lerp to
						playerToUpdate->SetNetworkedPosition(ParseVector(packetStrings[2]));
						//Set networked euler angles (which convert internally to a quaternion). Player will lerp rotation to this value
						playerToUpdate->SetNetworkedEulerAngles(ParseVector(packetStrings[3]));
						
						//Set networked scale for networked player. Lerp networked player scale to this value
						playerToUpdate->SetScale(ParseVector(packetStrings[4]));
						playerFound = true;
					}
				}
				//If the playerID doesn't match a local player
				if (!playerFound)
				{
					//If below the maximum number of players
					if (controllerManager->TotalControllers() < 4)
					{
						//Spawn a new player using the newly received position data
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
				//Set the player id for the local player
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
}

//Instantiate a player
Entity* NetworkManager::InstantiateNetworkedPlayer()
{
	return entityManager->InstantiateEntity(entityManager->LoadEntityFromFile("Resources/Prefabs/Player.prefab"), glm::vec3(), glm::vec3(0, 1, 0), 0.f, glm::vec3(1), nullptr);
}

//Sends a packet to our connected peer
bool NetworkManager::SendPacket(std::string * packetData)
{//Returns true if a packet is sent
	if (peer == nullptr)
		return false;
	ENetPacket* packet = enet_packet_create(packetData->c_str(), strlen(packetData->c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);

	return true;
}

//Parses a packet into a series of strings
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

//Connect to our server
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

	//TODO: Update this to use our server's IP
	enet_address_set_host(&address, "127.0.0.1");
	address.port = 70;

	//Try to connect to a host
	peer = enet_host_connect(client, &address, 1, 0);
	//If host connection fails
	if (peer == NULL)
	{
		std::cout << stderr << "No available peers for initiating ENET connection!" << std::endl;
		assert(0);
	}

	//If the server has connected back to us
	if (enet_host_service(client, &enetEvent, 1000) > 0 &&
		enetEvent.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to 127.0.0.1:70 succeeded!");
	}
	//If the server has failed to connect back with us
	else
	{
		enet_peer_reset(peer);
		puts("Connection to 127.0.0.1:70 failed.");
	}
}

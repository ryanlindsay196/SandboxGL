#pragma once
class ControllerPacketManager {
public:
	//void ReceiveInputFromClients(unsigned int lobbyID, unsigned int playerID);
	void SendInputToClients(unsigned int playerID);
};
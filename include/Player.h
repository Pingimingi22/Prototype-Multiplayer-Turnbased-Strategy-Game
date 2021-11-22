#pragma once
#include <string>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"

#include "InputChecking.h"

class Player
{
public:
	Player(std::string username);
	Player() {}

	bool m_IsServer = false;

	bool m_IsWorldCreated = false;

	RakNet::SystemAddress m_ServerAddress;

	bool InitNetworking(START_INPUT input);

	RakNet::SystemAddress ip;
	int m_PlayerTurnNum;

	std::string m_username;
	int m_food = 100;
	int m_wood = 100;
	int m_stone = 100;

	// Used when deciding who plays first.
	int m_rolledNumber;

	// Networking stuff:
	RakNet::RakPeerInterface* peer;

};
#pragma once
#include <string>

#include "external/raknet/Source/RakPeerInterface.h"
#include "external/raknet/Source/MessageIdentifiers.h"
#include "external/raknet/Source/RakNetTypes.h"

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
	int m_gold;
	int m_wood;
	int m_stone;

	// Used when deciding who plays first.
	int m_rolledNumber;

	// Networking stuff:
	RakNet::RakPeerInterface* peer;

};
#pragma once

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"

#include "Player.h"
#include "Building.h"
// Custom packet enums
enum
{
	ID_SEND_USERNAME = ID_USER_PACKET_ENUM,
	START_GAME,
	OTHER_JOINED,
	OTHER_CREATED_CASTLE,
	CREATED_CASTLE,
	ACK_SEND_USERNAME,
	FINISHED_WORLD_GENERATION,
	END_TURN,
	PRODUCTION_COMPLETE,
	UNIT_MOVE

};

#pragma pack(push, 1)
struct UsernamePacket
{
	unsigned char typeId = (unsigned char)ID_SEND_USERNAME;
	char username[50];
	// data here
};
#pragma pack(pop)

#pragma pack(push, 1)
struct AckUsernamePacket
{
	unsigned char typeId = (unsigned char)ACK_SEND_USERNAME;
	int playerID; // sending them their player turn number.
	// data here
};
#pragma pack(pop)

#pragma pack(push, 1)
struct StartGamePacket
{
	unsigned char typeId = (unsigned char)START_GAME;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct OtherPlayerJoinedPacket // To send to all other players that aren't the server when a new player joins.
{
	unsigned char typeId = (unsigned char)OTHER_JOINED;
	Player newPlayer;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CreatedCastlePacket 
{
	unsigned char typeId = (unsigned char)CREATED_CASTLE;
	
	char Username[50];

	bool CanBuild;
	bool IsMobile;

	int OwnerTurnID;
	Vector2 m_TileIndex;
	TileType TileType;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UnitProductionPacket
{
	unsigned char typeId = (unsigned char)PRODUCTION_COMPLETE;

	TileType ProduceTileType;
	Vector2 m_TileIndex;

	bool CanBuild;
	bool IsMobile;

	int OwnerTurnID;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct FinishedWorldGenerationPacket
{
	unsigned char typeId = (unsigned char)FINISHED_WORLD_GENERATION;

	int playerTurnID;
	bool IsWorldDone;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct EndTurnPacket
{
	unsigned char typeId = (unsigned char)END_TURN;
	int playerTurnID;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UnitMovePacket
{
	unsigned char typeId = (unsigned char)UNIT_MOVE;
	int playerTurnID;
	Vector2 movePos;
	
	// This position is used to find out which unit we are talking about.
	Vector2 tileOriginalPos;
};
#pragma pack(pop)


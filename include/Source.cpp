#include <iostream>

#include "Print.h"
#include "RakPeer.h"
#include "RakPeerInterface.h"

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include <string>
#include <algorithm>

#include "InputChecking.h"
#include "Player.h"
#include "MessageIdentifiers.h"

#include "PacketStructs.h"
#include <thread>

#include "Game.h"

#define sum(x, y) (x + y)

Player* localPlayer;
std::vector<Player> otherPlayers;

bool gameStarted = false;
bool hasInitGame = false;

Game* game;
int playerCount = 0;

Player* GetPlayerByTurnID(int id)
{
    if (localPlayer->m_PlayerTurnNum == id)
        return localPlayer;
    for (int i = 0; i < otherPlayers.size(); i++)
    {
        if (otherPlayers[i].m_PlayerTurnNum == id)
        {
            return &otherPlayers[i];
        }
    }
}

void OtherInputThread()
{
    while (true)
    {
        std::string input;
        std::getline(std::cin, input);
        std::transform(input.begin(), input.end(), input.begin(), toupper);
        if (input.compare("START") == 0)
        {
            std::cout << "Starting game..." << std::endl;
            // If we have started the game, that means we're the server and we have to init our game here.
            gameStarted = true;

            StartGamePacket pack;
            localPlayer->peer->Send((char*)&pack, sizeof(pack), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
            break;
        }
        else
        {
            std::cout << "That is not a valid command." << std::endl;
        }
    }
}


int main(int argc, char* args[])
{
    std::cout << "========================= Dan's Mini Survival Strategy Game ========================" << std::endl;
    std::cout << "Welcome user!" << std::endl;
    std::cout << "\nPlease enter your username: ";
    std::string enteredName;
    std::getline(std::cin, enteredName);
    std::cout << "Hello, " << enteredName << std::endl;

    localPlayer = new Player(enteredName);

    std::cout << "\nWill you be hosting or joining a game?" << std::endl;
    
    START_INPUT startInput = InputChecking::GetHostJoinInput();


    bool successfulCreation = false;
    switch (startInput)
    {
        case START_INPUT::HOST:
        {
            std::cout << "Setting up server." << std::endl;
            successfulCreation = localPlayer->InitNetworking(startInput);
            localPlayer->m_PlayerTurnNum = playerCount; // Host always is play 0.
            localPlayer->m_IsServer = true;
            playerCount++;
            break;
        }
        case START_INPUT::JOIN:
        {
            successfulCreation = localPlayer->InitNetworking(startInput);
            
            

            
            break;
        }
    }

    std::cout << "Type start, when you're ready to start the game." << std::endl;
    std::cout << "Waiting for players..." << std::endl;
    // FIXME make it so only the server can start the game.
    std::thread otherInput(OtherInputThread);

    SDL_Event e;
    while (true)
    {
        // If we reach this point, that means the server or client has been setup and we can start sending/receiving packets.
        RakNet::Packet* packet;
        for (packet = localPlayer->peer->Receive(); packet; localPlayer->peer->DeallocatePacket(packet), packet = localPlayer->peer->Receive())
        {
            unsigned char received = InputChecking::GetPacketIdentifier(packet);
            
            if (received == DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION)
            {
                std::cout << "Received incoming connection." << std::endl;
            }
            else if (received == DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED)
            {
                // Client was accepted by the server.
                UsernamePacket usernamePack;
                strcpy(usernamePack.username, enteredName.c_str());
                RakNet::SystemAddress testAddr("127.0.0.1", 25565);
                localPlayer->peer->Send((char*)&usernamePack, sizeof(usernamePack), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, testAddr, false);

                localPlayer->m_ServerAddress = packet->systemAddress;
            }
            else if (received == ID_SEND_USERNAME)
            {
                UsernamePacket* receivedUsername = (UsernamePacket*)packet->data;
                std::cout << receivedUsername->username << " has joined the server." << std::endl;

                // When receiving their username, create another player and put them into a collection of all players, also store their ip address.
                Player newPlayer = Player(receivedUsername->username);
                newPlayer.ip = packet->systemAddress;
                newPlayer.m_PlayerTurnNum = playerCount;
                playerCount++;

                

                AckUsernamePacket ackUsername;
                ackUsername.playerID = newPlayer.m_PlayerTurnNum;
                localPlayer->peer->Send((char*)&ackUsername, sizeof(AckUsernamePacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, packet->systemAddress, false);
                // Send the newly joined player their turn number.

                // Also send the newly joined player the knowledge of all other connected players, including the host.
                OtherPlayerJoinedPacket hostJoined;
                hostJoined.newPlayer = *localPlayer;
                localPlayer->peer->Send((char*)&hostJoined, sizeof(OtherPlayerJoinedPacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, packet->systemAddress, false);
                // Now to send knowledge about all other joined players.
                for (int i = 0; i < otherPlayers.size(); i++)
                {
                    OtherPlayerJoinedPacket previousPlayer;
                    previousPlayer.newPlayer = otherPlayers[i];
                    localPlayer->peer->Send((char*)&previousPlayer, sizeof(OtherPlayerJoinedPacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, packet->systemAddress, false);
                }


                // Now that we, the server, have received a message about a new player, we should tell all other players that this new player has joined.
                // send it to all other players.
                OtherPlayerJoinedPacket otherJoined;
                otherJoined.newPlayer = newPlayer;
                localPlayer->peer->Send((char*)&otherJoined, sizeof(OtherPlayerJoinedPacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, packet->systemAddress, true);
                // Broadcasts message to all other players.


                // Finally, add the new player to the player container.
                otherPlayers.push_back(newPlayer);

            }
            else if (received == ACK_SEND_USERNAME)
            {
                AckUsernamePacket* receivedAck = (AckUsernamePacket*)packet->data;
                std::cout << "Received our turn number. We are player number: " << receivedAck->playerID << std::endl;
                localPlayer->m_PlayerTurnNum = receivedAck->playerID;
            }
            else if (received == OTHER_JOINED)
            {
                OtherPlayerJoinedPacket* otherJoined = (OtherPlayerJoinedPacket*)packet->data;
                otherPlayers.push_back(otherJoined->newPlayer);
                std::cout << "Another player called " << otherJoined->newPlayer.m_username << " has joined the server." << std::endl;
            }
            else if (received == START_GAME)
            {
                std::cout << "Host has started the game!" << std::endl;

                // We start the game here because the host has started it.
                game = new Game(localPlayer, otherPlayers);
                //otherInput.join();
                game->Init(otherPlayers.size() + 1); // All other players plus us is the total number of players.
                gameStarted = true;
            }
            else if (received == CREATED_CASTLE)
            {
                CreatedCastlePacket* receivedCastle = (CreatedCastlePacket*)packet->data;

                Player* castleOwner = GetPlayerByTurnID(receivedCastle->OwnerTurnID);

                std::cout << "A castle has been constructed by " << castleOwner->m_username << "." << std::endl;
            
                // Making a copy of the received unit to store in our game.
                Unit* newCastleUnit = new Unit(game->GetTile(receivedCastle->m_TileIndex.x, receivedCastle->m_TileIndex.y), castleOwner, receivedCastle->CanBuild, receivedCastle->IsMobile, receivedCastle->TileType, receivedCastle->m_TileIndex.x, receivedCastle->m_TileIndex.y, 6);//hello

                newCastleUnit->m_Owner = castleOwner;
                newCastleUnit->AddButton(game->m_VillagerButton);
                
                game->m_AllUnits.push_back(newCastleUnit);

                // Now we have to place the castle down on the world map.
                game->PlaceUnit(newCastleUnit); // This also attached a reference to the unit for the tile.
            }
            else if (received == FINISHED_WORLD_GENERATION)
            {
                // A player has finished generating their world.
                FinishedWorldGenerationPacket* receivedPacket = (FinishedWorldGenerationPacket*)packet->data;

                for (int i = 0; i < otherPlayers.size(); i++)
                {
                    if (receivedPacket->playerTurnID == otherPlayers[i].m_PlayerTurnNum)
                    {
                        std::cout << otherPlayers[i].m_username << " has finished generating their world." << std::endl;
                        otherPlayers[i].m_IsWorldCreated = true;
                    }
                }
            }
            else if (received == END_TURN)
            {
                // A player has ended their turn. We will take what turn they say is next.
                std::cout << "Received end turn packet." << std::endl;
                EndTurnPacket* turnPacket = (EndTurnPacket*)packet->data;
                game->m_PlayerTurn = turnPacket->playerTurnID; // Set our player turn to the next player.

                // When we receive an END_TURN packet, that means a new turn has started. Here we will
                // update all of our unit structures productions.
                for (int i = 0; i < game->m_AllUnits.size(); i++)
                {
                    if (game->m_AllUnits[i]->m_Owner == game->m_LocalPlayer) // If we own the unit.
                    {
                        // The title the produce spawned on.
                        Tile spawnedOnTile;
                        Unit* producedUnit = nullptr;
                        if (game->m_AllUnits[i]->Produce(producedUnit, spawnedOnTile))
                        {
                            // Send unit production packet to all other clients.
                            UnitProductionPacket prodPack;
                            prodPack.m_TileIndex.x = spawnedOnTile.m_Node->m_XIndex;
                            prodPack.m_TileIndex.y = spawnedOnTile.m_Node->m_YIndex;
                            prodPack.ProduceTileType = Tile::ProductionTypeToTileType(game->m_AllUnits[i]->m_ProductionType);
                            
                            //game->m_AllUnits.push_back(producedUnit);
                            prodPack.CanBuild = producedUnit->m_CanBuild;
                            prodPack.IsMobile = producedUnit->m_IsMobile;
                            prodPack.OwnerTurnID = game->m_LocalPlayer->m_PlayerTurnNum;


                            localPlayer->peer->Send((char*)&prodPack, sizeof(UnitProductionPacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
                        }
                    }

                }

                // Also when we receive an END_TURN packet, it means our end turn button should re-enable.
                if (game->m_PlayerTurn == game->m_LocalPlayer->m_PlayerTurnNum)
                    game->m_EndTurnButton->m_Active = true;
            }
            else if (received == PRODUCTION_COMPLETE)
            {
                // A play has completed producing something. We have to draw a secondary sprite to what tile the produced item is on.
                std::cout << "Production complete packet received." << std::endl;

                UnitProductionPacket* prodPack = (UnitProductionPacket*)packet->data;

                Tile* tileToSet = game->GetTile(prodPack->m_TileIndex.x, prodPack->m_TileIndex.y);
                tileToSet->SetSecondaryTile(prodPack->ProduceTileType);

                Player* thePlayer;
                for (int i = 0; i < otherPlayers.size(); i++)
                {
                    if (otherPlayers[i].m_PlayerTurnNum == prodPack->OwnerTurnID)
                    {
                        thePlayer = &otherPlayers[i];
                        break;
                    }
                }

                Unit* newUnit = new Unit(tileToSet, thePlayer, prodPack->CanBuild, prodPack->IsMobile, prodPack->ProduceTileType, prodPack->m_TileIndex.x, prodPack->m_TileIndex.y, 5, true);
                game->m_AllUnits.push_back(newUnit);
                tileToSet->m_Game->PlaceUnit(newUnit);

            }
            else if (received == UNIT_MOVE)
            {
                // A player has moved one of their units, we need to move the unit for all clients.
                std::cout << "Received a unit move packet." << std::endl;

                UnitMovePacket* movePack = (UnitMovePacket*)packet->data;

                // We need to find what unit they moved, so we take the original tile position from the packet and find a matching unit.
                for (int i = 0; i < game->m_AllUnits.size(); i++)
                {
                    if (game->m_AllUnits[i]->m_Tile->m_Node->m_XIndex == movePack->tileOriginalPos.x && game->m_AllUnits[i]->m_Tile->m_Node->m_YIndex == movePack->tileOriginalPos.y)
                    {
                        // This is the unit that has been moved.
                        game->m_AllUnits[i]->Move(Dijkstra::GetShortestPath(game->m_AllUnits[i]->m_Tile->m_Node, game->GetTile(movePack->movePos.x, movePack->movePos.y)->m_Node));
                    }
                }

            }
            else
            {
                std::cout << "Received unknown message." << std::endl;
            }
                

        }

        if (gameStarted && !hasInitGame && localPlayer->m_IsServer)
        {
            game = new Game(localPlayer, otherPlayers);
            game->Init(otherPlayers.size() + 1); // All the other players plus us is the total player count.
            hasInitGame = true;
            // this happens when the host starts the game. we cant create the game object in the other thread.
        }

        if (gameStarted)
            game->Update(e);

        if (game != nullptr && game->hasQuit)
        {
            otherInput.join();
            return 0; // Exit out of game loop.
        }
    }

    return 0;


}



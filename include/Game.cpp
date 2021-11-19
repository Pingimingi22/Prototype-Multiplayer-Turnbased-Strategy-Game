#include "Game.h"
#include "SDL.h"
#include <iostream>
#include "SDL_main.h"
#include "SDL_image.h"

#include "Tmx.h"
#include "TmxMap.h"
#include "Grid.h"

#include "SDL_ttf.h"

#include <random>

#include "Vector2.h"
#include "Building.h"

#include "PacketStructs.h"


Game::Game(Player* localPlayer, std::vector<Player>& otherPlayers)
{ 
	m_LocalPlayer = localPlayer;
	m_OtherPlayers = &otherPlayers;
}

//void CloseSDL()
//{
//	SDL_DestroyRenderer
//}

SDL_Texture* Game::LoadTexture(std::string path)
{
	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface;
	loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		std::cout << "Unable to load image %s! SDL_image Error: %s\n" << path.c_str() << IMG_GetError() << std::endl;
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			std::cout << "Unable to create texture from %s! SDL Error: %s\n" << path.c_str() << SDL_GetError() << std::endl;
		}
		else
		{
			// FIXME
			// For some reason this crashes, look into it later.
			SDL_FreeSurface(loadedSurface);
		}
	}

	return newTexture;
}

bool Game::LoadMedia()
{
	bool success = false;

	gTexture = LoadTexture("images/villager-0001.png");
	if (gTexture == NULL)
	{
		std::cout << "Failed to load texture image!" << std::endl;
		success = false;
	}
	return success;
}

void Game::Init(int playerNum)
{
	m_PlayerCount = playerNum;

	bool success = true;
		
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not intialize! SDL_Error: " << SDL_GetError() << std::endl;
	}
	else
	{

		gWindow = SDL_CreateWindow("Prototype", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 950, SDL_WINDOW_SHOWN);
		if(gWindow == NULL)
		{
			success = false;
			std::cout << "Window could not be created! SDL Error: %s\n" << SDL_GetError() << std::endl;
		}
		else
		{
			// Window created successfully. now we create a renderer.
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				std::cout << "Renderer could not be created! SDL Error: %s\n" << SDL_GetError() << std::endl;
				success = false;
			}
			else
			{
				// Renderer created successfully so we can now initialize it.
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					std::cout << "SDL_image could not intialize! SDL_image Error:" << IMG_GetError() << std::endl;
					success = false;
				}

				if (TTF_Init() == -1)
				{
					std::cout << "SDL_ttf did not intialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
					success = false;
				}
			}
		}

		SDL_SetRenderDrawBlendMode(gRenderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);

		LoadMedia();


		map = new Tmx::Map();

		map->ParseFile("maps/test-rts-map.tmx");

		if (map->HasError())
		{
			std::cout << "Map loading error: " << map->GetErrorCode() << std::endl;
			std::cout << "error message: " << map->GetErrorText().c_str();
		}

		for (int i = 0; i < map->GetNumTilesets(); i++)
		{
			const Tmx::Tileset* tileset = map->GetTileset(i);
			if (tileset)
			{
				std::cout << "Name: " << tileset->GetName() << std::endl;
				//std::string imageSource = tileset->GetImage()->GetSource().c_str();
				//std::cout << "Image source: " << imageSource << std::endl;
			}

		}
		const Tmx::TileLayer* tilelayer = map->GetTileLayer(0);
		int count = 0;
		for (int i = 0; i < tilelayer->GetWidth(); i++)
		{
			for (int j = 0; j < tilelayer->GetHeight(); j++)
			{
				if (tilelayer->GetTileTilesetIndex(i, j) == -1)
				{
					std::cout << "No tile in this slot" << std::endl;
				}
				else
				{
					int id = tilelayer->GetTileId(i, j);
					std::cout << tilelayer->GetTileId(i, j) << std::endl;
				}
				count++;

			}
			//count++;
		}

		// ================= Initialising Navigation System ================= //
		// Creating NavMap.
		m_NavMap = new NavMap(32, 32);
		m_NavMap->ConnectNodes();
		// ================================================================== //

		m_MainFont = TTF_OpenFont("fonts/pixelplay.ttf", 14);
		if (m_MainFont == NULL)
		{
			std::cout << "Failed to load m_MainFont! SDL_ttf Error: " << TTF_GetError() << std::endl;
			success = false;
		}
	

		std::cout << "Num of tiles in world: " << count << std::endl;

		//testSprite = new Sprite("images/grass-001.png", 500, 500, 32, 32, gRenderer);
		//testSprite->SetPos(50, 50);

		m_GrassTexture = LoadTexture("images/grass-001.png");
		m_MountainTexture = LoadTexture("images/mountain-001.png");
		m_CastleTexture = LoadTexture("images/castle-neutral-001.png");							   
		m_ForestTexture = LoadTexture("images/forest-001.png");
		m_VillagerTexture = LoadTexture("images/villager-0001.png");


		m_WorldGrid = new Grid(tilelayer->GetWidth(), tilelayer->GetHeight(), 0, 0, gRenderer, this, m_NavMap);
		m_WorldGrid->Init(tilelayer);

		m_HotBar = new Sprite("images/hotbar-001.png", 0, 800, 1280, 150, gRenderer);
		m_FoodIconSprite = new Sprite("images/food-icon-001.png", 450, 795, 32, 32, gRenderer);
		m_StoneIconSprite = new Sprite("images/stone-icon-001.png", 600, 795, 32, 32, gRenderer);
		m_WoodIconSprite = new Sprite("images/wood-icon-001.png", 750, 795, 32, 32, gRenderer);


		m_SelectTexture = new Sprite("images/select-001.png", 0, 0, 1280/32, 800/32, gRenderer);

		SDL_Color testColour;
		testColour.r = 0;
		testColour.g = 0;
		testColour.b = 1;
		m_TileSelectText = Text("-", testColour, m_MainFont, gRenderer);


		// ======================== Creating Unit Button Templates. ======================== //
		// The game holds templates of these buttons and each unit creates it's own button.
		// This is so each button that produces a unit can be attached to a specific unit.
		Sprite* activeVillager = new Sprite("images/villager-button-001.png", 900, 845, 64, 64, gRenderer);
		Sprite* inactiveVillager = new Sprite("images/villager-button-inactive-001.png", 900, 845, 64, 64, gRenderer);
		m_VillagerButton = new Button(activeVillager, inactiveVillager, true, this, BUTTON_TYPE::PRODUCE);
		// ======================================================================= //




		if(m_LocalPlayer->m_IsServer)
			GenerateSpawnLocations(); // generate them only if all players have finished loading in.

		if (!m_LocalPlayer->m_IsServer) // if we aren't the server, lets tell the server when we're done generating the world.
			SendWorldGenerationComplete();

		//if (m_LocalPlayer->m_IsServer)
		//{
			// If we're the server, make a green end turn button (because it's our turn.)
			Sprite* activeSprite = new Sprite("images/end-turn-active-001.png", 600, 845, 128, 64, gRenderer);
			Sprite* inactiveSprite = new Sprite("images/end-turn-inactive-001.png", 600, 845, 128, 64, gRenderer);
			m_EndTurnButton = new Button(activeSprite, inactiveSprite, true, this, BUTTON_TYPE::END_TURN);
			m_AllButtons.push_back(m_EndTurnButton); // Probably should remember to clean up memory here and basically everywhere else in the codebase..

			// Setting special reference for the end turn button.
			//m_EndTurnButton = 
		//}

			
			m_CurrentTime = SDL_GetPerformanceCounter();
			m_DeltaTime = 0;
		
	}



	
}

void Game::Update(SDL_Event& e)
{
	m_PrevTime = m_CurrentTime;
	m_CurrentTime = SDL_GetPerformanceCounter();
	
	m_DeltaTime = (float)((m_CurrentTime - m_PrevTime) * 1000 / (float)SDL_GetPerformanceFrequency());
	//m_DeltaTime *= 0.001; // Make it into seconds.
	//std::cout << m_DeltaTime << std::endl;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			hasQuit = true;
		}
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			std::cout << "Mouse button down." << std::endl;
		}

		m_WorldGrid->HandleInput(e);
		for (int i = 0; i < m_AllButtons.size(); i++)
			m_AllButtons[i]->HandleInput(e);
	}

	if (!m_AllPlayersGenerationCompleted && m_LocalPlayer->m_IsServer)
	{
		if (CheckPlayerGeneration())
		{
			m_AllPlayersGenerationCompleted = true;
			for (int i = 0; i < m_AllUnits.size(); i++)
			{
				SendSpawnLocation(*(Building*)m_AllUnits[i]); // After all player's have generated, send them the spawn locations.
			}
		}
	}

	// While the user hasn't quit.

	SDL_RenderClear(gRenderer);

	//SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
	
	//testSprite->Draw();

	m_WorldGrid->Update();

	m_HotBar->Draw(gRenderer);
	m_WorldGrid->Draw(gRenderer);
	m_SelectTexture->Draw(gRenderer);

	m_TileSelectText.Draw(gRenderer);
	m_FoodIconSprite->Draw(gRenderer);
	m_StoneIconSprite->Draw(gRenderer);
	m_WoodIconSprite->Draw(gRenderer);

	for (int i = 0; i < m_AllButtons.size(); i++)
	{
		m_AllButtons[i]->Draw();
	}
	SDL_RenderPresent(gRenderer);

	
}

/// <summary>
/// Only the server calls this.
/// </summary>
void Game::GenerateSpawnLocations()
{
	std::vector<Vector2> usedCoords;
	// Generate a location for yourself, as the server.


	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, 31);
	int randomX = distribution(generator);
	int randomY = distribution(generator);

	Vector2 serverCoords(randomX, randomY);
	Tile* serversTile = GetTile(randomX, randomY);
	
	// Local function cache to prevent generating same random number twice.
	usedCoords.push_back(serverCoords);

	Building* newBuilding = new Building(serversTile, m_LocalPlayer, true, false, TileType::CASTLE, randomX, randomY, 6);
	newBuilding->AddButton(*m_VillagerButton);
	// Adding to container of all game units.
	m_AllUnits.push_back(newBuilding);

	// Tile initialising stuff.
	serversTile->SetTile(TileType::CASTLE);
	serversTile->AttachUnit(newBuilding);

	for (int i = 0; i < m_OtherPlayers->size(); i++)
	{
		// Now do the same but for all other players.
		bool success = false;
		while (!success)
		{
			success = true;

			int otherRandomX = distribution(generator);
			int otherRandomY = distribution(generator);
			Vector2 otherPlayerCoords(otherRandomX, otherRandomY);
			
			for (int j = 0; j < usedCoords.size(); j++)
			{
				if (otherPlayerCoords == usedCoords[j])
				{
					// We have already used these coords, so redo this loop.
					success = false;
				}
			}
			if (success == false)
				continue; // Redo this loop of the while loop.

			// If we get here we have generated a vector coordinate that no other player has used.

			usedCoords.push_back(otherPlayerCoords);
			Tile* otherPlayerTile = GetTile(otherRandomX, otherRandomY);

			Building* newBuilding = new Building(otherPlayerTile, &m_OtherPlayers->at(i), true, false, TileType::CASTLE, otherRandomX, otherRandomY, 6);
			

			m_AllUnits.push_back(newBuilding);

			otherPlayerTile->SetTile(newBuilding->m_TileType);
			otherPlayerTile->AttachUnit(newBuilding);

			//SendSpawnLocation(*newBuilding); we are now sending this information else where.

		}
		
	}
}

Tile* Game::GetTile(int x, int y)
{
	return m_WorldGrid->GetTile(x, y);
}

void Game::SetTile(int x, int y, TileType type)
{
	Tile* tile = GetTile(x, y);
	tile->SetTile(type);
}
void Game::SetSecondaryTile(int x, int y, TileType type)
{
	Tile* tile = GetTile(x, y);
	tile->SetSecondaryTile(type);
}

void Game::SendSpawnLocation(Building spawnedCastle)
{
	CreatedCastlePacket createdCastle;
	createdCastle.CanBuild = spawnedCastle.m_CanBuild;
	createdCastle.IsMobile = spawnedCastle.m_IsMobile;
	createdCastle.OwnerTurnID = spawnedCastle.m_OwnerTurnID;
	createdCastle.m_TileIndex = spawnedCastle.m_TileIndex;
	createdCastle.TileType = spawnedCastle.m_TileType;
	strcpy(createdCastle.Username, spawnedCastle.m_Owner->m_username.c_str());
	

	m_LocalPlayer->peer->Send((char*)&createdCastle, sizeof(CreatedCastlePacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);

}

void Game::PlaceUnit(Unit* unit)
{
	Tile* tile = GetTile(unit->m_TileIndex.x, unit->m_TileIndex.y);
	tile->AttachUnit(unit);
	//unit->m_Tile->AttachUnit(unit);

	// Setting the texture of the tile. If we are producing a unit that overlays an additional texture over the
	// regular tile, we want to call SetSecondaryTile().

	if (unit->m_HasSecondaryTile)
	{
		SetSecondaryTile(unit->m_TileIndex.x, unit->m_TileIndex.y, unit->m_TileType);
	}
	else
	{
		SetTile(unit->m_TileIndex.x, unit->m_TileIndex.y, unit->m_TileType);
	}

}

void Game::SendWorldGenerationComplete()
{
	FinishedWorldGenerationPacket generationPacket;
	generationPacket.IsWorldDone = true;
	generationPacket.playerTurnID = m_LocalPlayer->m_PlayerTurnNum;

	

	m_LocalPlayer->peer->Send((char*)&generationPacket, sizeof(FinishedWorldGenerationPacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, m_LocalPlayer->m_ServerAddress, false);
}

bool Game::CheckPlayerGeneration()
{
	for (int i = 0; i < m_OtherPlayers->size(); i++)
	{
		if (m_OtherPlayers[0][i].m_IsWorldCreated == false)
			return false;
	}
	return true;
}

void Game::EndTurn()
{
	// When the turns ends, update all units so that the production intervals get set.
	for (int i = 0; i < m_AllUnits.size(); i++)
	{
		// We only update the production if it is our turn that has ended.
		if (m_PlayerTurn == m_LocalPlayer->m_PlayerTurnNum)
			m_AllUnits[i]->UpdateProduction();
	}



	// We end our turn, increment the turn number. Then we send an end turn packet to everybody.

	if (m_PlayerTurn == m_PlayerCount - 1) // If it's the last persons turn, set the player turn back to 0. It's minus 1 because player turns start at 0.
	{
		m_PlayerTurn = 0;
	}
	else // Otherwise, just increment it, passing it to the next player.
	{
		m_PlayerTurn++; // Increment turn, so that way it'll be the next persons turn.
	}
	EndTurnPacket packet;
	packet.playerTurnID = m_PlayerTurn;

	// Try to send to everyone.
	m_LocalPlayer->peer->Send((char*)&packet, sizeof(EndTurnPacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);

	

	if (m_OtherPlayers->size() == 0) // If we are playing just by ourselves, for debugging purposes, check all units produce when we end the turn.
	{							     // Normally this is done upon receiving an END_TURN packet but we wont be receiving any if we are playing solo.

		for (int i = 0; i < m_AllUnits.size(); i++)
		{
			if (m_PlayerTurn == m_LocalPlayer->m_PlayerTurnNum) // This if check is redundant because there is only us in the game.
				if(m_AllUnits[i]->Produce());
		}
	}
}


void Game::SendUnitMove(Vector2 posToGoTo, Unit* unit)
{
	UnitMovePacket packet;
	packet.movePos = posToGoTo;
	packet.tileOriginalPos = { unit->m_Tile->m_Node->m_XIndex, unit->m_Tile->m_Node->m_YIndex };

	m_LocalPlayer->peer->Send((char*)&packet, sizeof(UnitMovePacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
}
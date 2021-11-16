#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <string>

#include "Tmx.h"

#include "Sprite.h"

#include "Grid.h"
#include "SDL_ttf.h"

#include "Text.h"

#include "Player.h"

#include "Unit.h"
#include "Building.h"
#include "Button.h"


#include "NavMap.h"
#include "Dijkstra.h"


class Game
{
public:
	bool hasQuit = false;

	Player* m_LocalPlayer;
	std::vector<Player>* m_OtherPlayers;

	bool m_AllPlayersGenerationCompleted = false;

	int m_PlayerCount;

	int m_PlayerTurn = 0; // Whose turn it is currently.

	SDL_Renderer* gRenderer;
	SDL_Window* gWindow;

	SDL_Texture* gTexture;

	Tmx::Map* map;

	Sprite* testSprite;
	Sprite* m_FoodIconSprite;
	Sprite* m_StoneIconSprite;
	Sprite* m_WoodIconSprite;

	Grid* m_WorldGrid;

	Sprite* m_HotBar;

	Sprite* m_SelectTexture;

	Tile* m_CurrentlySelectedTile = nullptr;

	// ========================= General Buttons ========================= //
	std::vector<Button*> m_AllButtons;
	// End turn button has a special reference because we need to use it a lot.
	Button* m_EndTurnButton;
	// =================================================================== //

	TTF_Font* m_MainFont;

	Text m_TileSelectText;

	std::vector<Unit*> m_AllUnits;

	SDL_Texture* m_GrassTexture = NULL;
	SDL_Texture* m_MountainTexture;
	SDL_Texture* m_ForestTexture = NULL;
	SDL_Texture* m_CastleTexture;
	SDL_Texture* m_VillagerTexture;

	// =================== Unit amd Building Buttons =================== //
	Button* m_VillagerButton;
	
	// ================================================================= //

	// =================== Navigation System =================== //
	NavMap* m_NavMap;
	// ========================================================= //

	Game(Player* localPlayer, std::vector<Player>& otherPlayers);
	void Init(int playerNum);
	void Update(SDL_Event& e);
	bool LoadMedia();
	SDL_Texture* LoadTexture(std::string path);

	bool CheckPlayerGeneration();

	void GenerateSpawnLocations();
	void SendSpawnLocation(Building spawnedCastle);
	void SendWorldGenerationComplete();
	void EndTurn();

	void PlaceUnit(Unit* unit);


	Tile* GetTile(int x, int y);

	void SetTile(int x, int y, TileType type);





};
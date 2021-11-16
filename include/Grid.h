#pragma once
#include "SDL.h"
#include "Tile.h"
#include <vector>
#include "Tmx.h"

class NavMap;
class Game;
class Grid
{
public:

	std::vector<std::vector<Tile*>> m_TileMap;
	float m_Width;
	float m_Height;
	SDL_Renderer* m_Renderer;

	Game* m_Game;

	// Reference to the NavMap.
	NavMap* m_NavMap;

	Grid(float width, float height, float xSpacing, float ySpacing, SDL_Renderer* renderer, Game* game, NavMap* navMap);

	void Init(const Tmx::TileLayer* tileLayer);

	void HandleInput(SDL_Event& e);

	void Draw(SDL_Renderer* renderer);
	void Update();

	Tile* GetTile(int x, int y);

};
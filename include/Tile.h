#pragma once
#include "SDL.h"
#include "Sprite.h"
#include "Node.h"
#include "ProductionTypes.h"

class Unit;
class Game;
class Grid;
enum TileType
{
	// The numbers match the corresponding ID from the Tiled editor.
	FOREST = 4,
	GRASS = 5,
	MOUNTAIN = 8,
	CASTLE,
	VILLAGER
};
enum TileID
{


};

class Tile
{
public:
	Sprite* m_Sprite;
	TileType m_Type;
	SDL_Renderer* m_Renderer;
	float m_xPos;
	float m_yPos;
	float m_Width;
	float m_Height;

	// Reference back to the world grid so we can access neighbouring tiles and stuff.
	Grid* m_WorldGrid;

	// ======= Secondary Tile Stuff ======= //
	bool m_HasSecondaryTile = false;
	TileType m_TypeSecondary;
	Sprite* m_SpriteSecondary;
	// ==================================== //

	bool m_IsHighlighted = false;
	SDL_Color m_HighlightColour;
	


	Game* m_Game = nullptr;

	Unit* m_Unit = nullptr;

	Tile();
	Tile(TileType type, float xPos, float yPos, float width, float height, SDL_Renderer* renderer, Game* game, Grid* worldGrid);


	// ============= References To Navigation System ============= //
	Node* m_Node;
	// =========================================================== //

	//Tile(const Tile& copy)
	//{
	//	//m_Sprite = copy.m_Sprite;
	//	//m_Sprite = copy.m_Sprite;
	//	//m_Type = copy.m_Type;
	//	//m_Renderer = copy.m_Renderer;
	//	//m_xPos = copy.m_xPos;
	//	//m_yPos = copy.m_yPos;
	//	//m_Width = copy.m_Width;
	//	//m_Height = copy.m_Height;
	//	//m_Game = copy.m_Game;
	//	//m_Unit = copy.m_Unit;
	//}

	//Tile& operator = (const Tile& assignment)
	//{
	//	//m_Sprite = assignment.m_Sprite;
	//	//m_Type = assignment.m_Type;
	//	//m_Renderer = assignment.m_Renderer;
	//	//m_xPos = assignment.m_xPos;
	//	//m_yPos = assignment.m_yPos;
	//	//m_Width = assignment.m_Width;
	//	//m_Height = assignment.m_Height;
	//	//m_Game = assignment.m_Game;
	//	//m_Unit = assignment.m_Unit;
	//}

	void Draw(SDL_Renderer* renderer);

	void HandleInput(SDL_Event& e);
	

	std::string GetName();

	std::string GetTilePath(TileType type);
	SDL_Texture* GetCachedTexture(TileType type);

	
	void SetTile(TileType type);
	/// <summary>
	/// Sets an additional sprite to be rendered for this tile. Used when drawing villagers onto other tiles.
	/// </summary>
	/// <param name="type">Type of tile to draw.</param>
	void SetSecondaryTile(TileType type);

	/// <summary>
	/// Removes the current secondary tile.
	/// </summary>
	void RemoveSecondaryTile();


	void AttachUnit(Unit* unit);
	void ClearUnit();

	/// <summary>
	/// Used to check if the surrounding 8 tiles (includes diagonals) are clear. If atleast one tile is clear this function will return
	/// true. Used when producing items from units. The additional parameter is incase you want to grab a reference the first available tile.
	/// </summary>
	/// <param name="output">Outputs the first available tile.</param>
	/// <returns>Returns true if atleast 1 tile is available.</returns>
	bool CheckSurroundingTiles(Tile& output);

	/// <summary>
	/// Converts TileType to equivalent PRODUCTION_TYPE.
	/// </summary>
	/// <param name="type">PRODUCTION_TYPE that will be converted to TileType.</param>
	/// <returns></returns>
	static TileType ProductionTypeToTileType(PRODUCTION_TYPE type);


	void SetHighlight(Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool toggle);
	void Highlight();

	bool CheckIfPassable();

};
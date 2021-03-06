#pragma once
#include "Tile.h"
#include "Player.h"
#include "Vector2.h"
#include <vector>


#include "ProductionTypes.h"

class Button;
class Unit
{
public:
	Unit() {}
	Unit(Tile* attachedTile, Player* owner, bool canBuild, bool isMobile, TileType tileType, float tileXIndex, float tileYIndex, float reach, bool secondaryTile = false)
	{
		m_Tile = attachedTile;
		m_Owner = owner;
		m_CanBuild = canBuild;
		m_IsMobile = isMobile;
		m_AttachedTilePos = Vector2(m_Tile->m_xPos, m_Tile->m_yPos);
		m_OwnerAddress = m_Owner->ip;
		m_TileType = tileType;
		m_OwnerTurnID = m_Owner->m_PlayerTurnNum;

		m_TileIndex.x = tileXIndex;
		m_TileIndex.y = tileYIndex;

		m_Reach = reach;

		m_HasSecondaryTile = secondaryTile;

		m_HealthBarRect.x = m_Tile->m_xPos;
		m_HealthBarRect.y = m_Tile->m_yPos + 20;
		m_HealthBarRect.h = 5;
	}

	~Unit();

	//bool m_IsPlacing = false; // moved to Game.cpp

	float m_Health = 100;
	SDL_Rect m_HealthBarRect;

	bool m_HasSecondaryTile = false;
	bool m_CanBuild;
	bool m_IsMobile;
	// Reach for the walkable tiles that this unit can walk to or reach.
	float m_Reach = 0;

	bool m_HasMovedThisTurn = false;
	bool m_HasAttackedThisTurn = false;

	// have to rehook these up after sending through the network.
	// Not to be used over network!
	Tile* m_Tile;
	// Not to be used over network!
	Player* m_Owner;

	// Use these when transmitting over the network!
	Vector2 m_TileIndex;
	RakNet::SystemAddress m_OwnerAddress;
	int m_OwnerTurnID;
	Vector2 m_AttachedTilePos;
	TileType m_TileType;

	// Container for all the buttons attached to this unit.
	std::vector<Button*> m_Buttons;


	// Container for all tiles that player can move to. Including the tiles that are inpassable and will show as red. This is only for mobile units..
	std::vector<Tile*> m_WalkableTiles;

	// Container for tiles that the unit can currently attack.
	std::vector<Tile*> m_AttackableTiles;

	bool m_IsMoving = false;
	int m_CurrentPathIndex = 0;
	// Path that the unit is currently following.
	std::vector<Tile*> m_CurrentPath;
	// Time the unit has been moving for.
	float m_ElapsedMoveTime = 0; 

	// ============= Unit Production Stuff ============= //
	PRODUCTION_TYPE m_ProductionType;
	bool m_IsProducing = false;
	int m_ProductionTurnsElapsed;


	void HandleInput(SDL_Event& e);

	void Place(Tile* tileToPlaceOn);
	void RemoveResources(TileType typeOfBuilding);

	/// <summary>
	/// Call to make unit start production of specified type.
	/// </summary>
	/// <param name="type">Type of produce.</param>
	void StartProduction(PRODUCTION_TYPE type);

	/// <summary>
	/// Checks if the currently produced item is ready to be produced. This should be called when a new turn starts and the turn number is
	/// equal to the local player's number.
	/// </summary>
	/// <returns></returns>
	bool CheckProduction();

	/// <summary>
	/// Produces the worked on production type. Returns true if successful and false if unsuccessful. Units
	/// wont be produced if there are no surrounding tiles for them to spawn on. output parameter allows user
	/// to return the tile the produced item spawned on.
	/// </summary>
	/// <param name="output">Reference to the tile which the produce spawned on.</param>
	/// <returns></returns>
	bool Produce(Unit*& outputUnit, Tile& output = Tile());

	/// <summary>
	/// To be called when the turn starts. This sets the number of production turns left.
	/// </summary>
	void UpdateProduction();

	/// <summary>
	/// Gets the turns required to produced specified type.
	/// </summary>
	/// <param name="type">Type of which you want to find out how many turns are required to produce it.</param>
	/// <returns></returns>
	int GetProductionTurns(PRODUCTION_TYPE type);
	// ================================================= //

	void ToggleButtons(bool toggle);

	void Select();
	void Destroy();

	/// <summary>
	/// Creates a new button and adds it to the m_Buttons std::vector. The button will appear when the user clicks on this unit.
	/// </summary>
	void AddButton(Button* button);
	void DrawButtons();

	void CalculateWalkableTiles(bool ignorePassability = false);
	

	void HighlightWalkable();
	void Unhighlight();

	void Move(std::vector<Node*> navPath);

	// Called to move unit along path.
	void MoveTo(int pathIndex);

	void UpdateMove();

	bool IsNodeReachable(Node* node);

	/// <summary>
	/// Checks if the unit can place it's production type on the passed in tile type.
	/// </summary>
	/// <param name="tileType">Type of the tile that you want to place the unit on.</param>
	/// <returns>True is returned if it can place the production type on the passed in tile.</returns>
	bool CheckCanPlace(TileType tileType);

	/// <summary>
	/// Checks if we can afford the passed in tile type as a building to place.
	/// </summary>
	/// <param name="buildingType">Tile type of the building. Should be either QUARRY, LUMBER or FARM.</param>
	/// <returns>True if we can afford the specified building.</returns>
	bool CanAfford(TileType buildingType);

	void GenerateResources();
	
	void TakeDamage(float damage);
	void DrawHealth();

	void UpdateHealthBarPos();

	void CalculateAttackTiles(float gScoreReach);
	bool CanAttack(Tile* tile);


};
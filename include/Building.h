#pragma once
#include "Unit.h"
#include "Tile.h"


class Building : public Unit
{
public:
	Building() {}
	Building(Tile* attachedTile, Player* owner, bool canBuild, bool isMobile, TileType tileType, float tileXIndex, float tileYIndex, unsigned int reach) : Unit(attachedTile, owner, canBuild, isMobile, tileType, tileXIndex, tileYIndex, reach) {}

	

};
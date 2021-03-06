#include "Unit.h"
#include "Button.h"
#include "Grid.h"
#include "Game.h"

#include "Dijkstra.h"

#include "PacketStructs.h"

#include "BuildingCosts.h"
#include "BuildingProductionStats.h"

// =============== Defines of some general gameplay stuff =============== //
#define VILLAGER_PRODUCTION_TURNS 2 // Takes 2 turns to produce one villager.
#define LUMBER_PRODUCTION_TURNS 0
#define QUARRY_PRODUCTION_TURNS 0
#define FARM_PRODUCTION_TURNS 0

// ====================================================================== //



Unit::~Unit()
{

	m_Tile->ClearUnit();
	m_Tile = nullptr;

	// Button's are not ours to delete, they are created in Game.cpp and should be deleted there.
	/*for (int i = m_Buttons.size() - 1; i > 0; i--)
	{
		delete m_Buttons[i];
	}*/

	m_Buttons.clear();
	m_WalkableTiles.clear();
	m_AttackableTiles.clear();
	m_CurrentPath.clear();
}



void Unit::AddButton(Button* button)
{
	//Button* newButton = new Button(button.m_ActiveSprite, button.m_InactiveSprite, true, button.m_Game, button.m_Type, this); // Important last parameter.
																															    // this keyword means this unit
																															    // will be attached to the button.
	m_Buttons.push_back(button);
}

void Unit::DrawButtons()
{
	for (int i = 0; i < m_Buttons.size(); i++)
	{
		m_Buttons[i]->Draw();
	}
}

void Unit::RemoveResources(TileType typeOfBuilding)
{
	switch (typeOfBuilding)
	{
	case TileType::FARM:
		m_Tile->m_Game->RemoveResource(FARM_WOOD_COST, FARM_FOOD_COST, FARM_STONE_COST);
		break;
	case TileType::LUMBER:
		m_Tile->m_Game->RemoveResource(LUMBER_WOOD_COST, LUMBER_FOOD_COST, LUMBER_STONE_COST);
		break;
	case TileType::QUARRY:
		m_Tile->m_Game->RemoveResource(QUARRY_WOOD_COST, QUARRY_FOOD_COST, QUARRY_STONE_COST);
		break;
	}

	m_Tile->m_Game->UpdateResourceText();

	// Now that we've deducted resources, we have to check if we need to disable any building buttons since the
	// player might not have enough to afford them.
	for (int i = 0; i < m_Buttons.size(); i++)
	{
		if (!CanAfford(m_Tile->ProductionTypeToTileType(m_Buttons[i]->m_prodType)))
		{
			m_Buttons[i]->m_Active = false;
		}
	}


}

void Unit::Place(Tile* tileToPlaceOn)
{
	TileType newTileType = m_Tile->ProductionTypeToTileType(m_ProductionType);

	// Deducting resources from player.
	RemoveResources(newTileType);

	Unit* newUnit = new Unit(tileToPlaceOn, m_Owner, false, false, newTileType, tileToPlaceOn->m_Node->m_XIndex, tileToPlaceOn->m_Node->m_YIndex, 0, false);

	m_Tile->m_Game->m_AllUnits.push_back(newUnit);

	m_Tile->m_Game->PlaceUnit(newUnit, false);

	m_Tile->m_Game->m_IsPlacing = false;

	UnitProductionPacket prodPack;
	prodPack.CanBuild = newUnit->m_CanBuild;
	prodPack.IsMobile = newUnit->m_IsMobile;
	prodPack.m_TileIndex = newUnit->m_TileIndex;
	prodPack.OwnerTurnID = newUnit->m_OwnerTurnID;
	prodPack.ProduceTileType = m_Tile->ProductionTypeToTileType(m_ProductionType);
	prodPack.Reach = newUnit->m_Reach;

	m_Tile->m_Game->m_LocalPlayer->peer->Send((char*)&prodPack, sizeof(UnitProductionPacket), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
	
}

void Unit::StartProduction(PRODUCTION_TYPE type)
{
	m_ProductionType = type;
	m_IsProducing = true;
	m_ProductionTurnsElapsed = 0;

	// When you start producing something from this unit, all buttons for this unit will be disabled.
	ToggleButtons(false);
}

bool Unit::Produce(Unit*& outputUnit, Tile& output)
{
	if (CheckProduction()) // We can try and produce this item.
	{
		Tile availableTile;
		// Check surrounding tiles and if atleast 1 spot is clear, produce item and return true;
		if (m_Tile->CheckSurroundingTiles(availableTile))
		{
			TileType newTileType = m_Tile->ProductionTypeToTileType(m_ProductionType);

			// We have atleast one spot to put the produced item.
			Unit* newUnit = new Unit(m_Tile->m_Game->GetTile(availableTile.m_Node->m_XIndex, availableTile.m_Node->m_YIndex), m_Owner, true, true, newTileType, availableTile.m_Node->m_XIndex, availableTile.m_Node->m_YIndex, 3.14f, true);
			newUnit->AddButton(m_Tile->m_Game->m_LumberButton);
			newUnit->AddButton(m_Tile->m_Game->m_QuarryButton);
			newUnit->AddButton(m_Tile->m_Game->m_FarmButton);
			outputUnit = newUnit;


			m_Tile->m_Game->m_AllUnits.push_back(newUnit);
			
			m_Tile->m_Game->PlaceUnit(newUnit);


			TileType tileType = Tile::ProductionTypeToTileType(m_ProductionType);

			
			//availableTile.SetSecondaryTile(tileType);

			m_ProductionTurnsElapsed = 0;
			m_IsProducing = false;

			// Turn on all the unit's buttons.
			ToggleButtons(true);

			output = availableTile;

			// Re-generate walkable tiles.
			CalculateWalkableTiles();

			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool Unit::CheckProduction()
{
	if (m_IsProducing)
	{
		if (m_ProductionTurnsElapsed >= GetProductionTurns(m_ProductionType))
		{
			// We are ready to attempt to produce this unit.
			return true;
		}
		else
			return false;
	}
	return false;
}

void Unit::UpdateProduction()
{
	if (m_IsProducing)
	{
		m_ProductionTurnsElapsed++;
	}
	else // If we aren't producing anything, just set production turns to 0 just to be safe.
		m_ProductionTurnsElapsed = 0;

}

int Unit::GetProductionTurns(PRODUCTION_TYPE type)
{
	switch (type)
	{
	case PRODUCTION_TYPE::VILLAGER:
		return VILLAGER_PRODUCTION_TURNS;
	case PRODUCTION_TYPE::LUMBER:
		return LUMBER_PRODUCTION_TURNS;
	case PRODUCTION_TYPE::QUARRY:
		return QUARRY_PRODUCTION_TURNS;
	case PRODUCTION_TYPE::FARM:
		return FARM_PRODUCTION_TURNS;
	default:
		std::cout << "Unknown type passed in to GetProductionTurns()" << std::endl;
		return -1;
	}
}

void Unit::ToggleButtons(bool toggle)
{
	for (int i = 0; i < m_Buttons.size(); i++)
	{
		m_Buttons[i]->m_Active = toggle;
	}
}

void Unit::CalculateWalkableTiles(bool ignorePassability)
{
	m_WalkableTiles.clear(); // Clear previous calculation.

	std::vector<Node*> nodesUnderGScore = Dijkstra::GetNodesUnderGScore(m_Reach, m_Tile->m_Node, ignorePassability);
	for (int i = 0; i < nodesUnderGScore.size(); i++)
	{
		m_WalkableTiles.push_back(m_Tile->m_Game->GetTile(nodesUnderGScore[i]->m_XIndex, nodesUnderGScore[i]->m_YIndex));
	}

	//Node* currentNode = m_Tile->m_Node;

	//unsigned int currentReachLevel = 0;

	//std::vector<Node*> currentLevel;

	//// Get the initial 8 tiles.
	//for (int i = 0; i < currentNode->m_ConnectedNodes.size(); i++)
	//{
	//	Node* nodeToAdd = currentNode->m_ConnectedNodes[i];

	//	Tile* tile = m_Tile->m_Game->GetTile(nodeToAdd->m_XIndex, nodeToAdd->m_YIndex);

	//	currentLevel.push_back(nodeToAdd);
	//	m_WalkableTiles.push_back(tile);
	//}
	//currentReachLevel++; // We increment reach level to 1.


	//std::vector<Node*> temp; // This should contain the outer ring which represents the next level.
	//while (currentReachLevel < m_Reach)
	//{
	//	for (int i = 0; i < currentLevel.size(); i++)
	//	{
	//		currentNode = currentLevel[i];
	//		for (int j = 0; j < currentNode->m_ConnectedNodes.size(); j++)
	//		{
	//			temp.push_back(currentNode->m_ConnectedNodes[j]);
	//		}
	//	}
	//	
	//	currentLevel.clear();
	//	for (int i = 0; i < temp.size(); i++)
	//	{
	//		currentLevel.push_back(temp[i]);
	//		m_WalkableTiles.push_back(m_Tile->m_Game->GetTile(temp[i]->m_XIndex, temp[i]->m_YIndex));
	//	}
	//	temp.clear();

	//	currentReachLevel++;
	//}
}

void Unit::HighlightWalkable()
{
	for (int i = 0; i < m_WalkableTiles.size(); i++)
	{
		Tile* currentTile = m_WalkableTiles[i];

		if (m_HasMovedThisTurn || m_Tile->m_Game->m_LocalPlayer->m_PlayerTurnNum != m_Tile->m_Game->m_PlayerTurn)
		{
			m_WalkableTiles[i]->SetHighlight(125, 125, 125, 100, true);
		}
		else
		{
			// If we are placing a tile, we want to only highligh the tiles that the tile can be placed on.
			if (m_Tile->m_Game->m_IsPlacing)
			{
				if (CheckCanPlace(m_WalkableTiles[i]->m_Type))
					m_WalkableTiles[i]->SetHighlight(0, 0, 255, 100, true);
				else
					m_WalkableTiles[i]->SetHighlight(255, 0, 0, 100, true);
			}
			else
			{
				if (currentTile->m_Node->m_Passable)
				{
					if (m_Owner == m_Tile->m_Game->m_LocalPlayer) // If we own it, show walkable tiles as blue
						m_WalkableTiles[i]->SetHighlight(0, 0, 255, 100, true); // Highlight blue if passable.
					else // Otherwise, show walkable tiles as yellow.
						m_WalkableTiles[i]->SetHighlight(255, 241, 161, 100, true);
				}
				else
					m_WalkableTiles[i]->SetHighlight(255, 0, 0, 100, true); // Red if not passable.
			}
		}

	}
}

void Unit::Unhighlight()
{
	for (int i = 0; i < m_WalkableTiles.size(); i++)
	{
		m_WalkableTiles[i]->m_IsHighlighted = false;
	}
}

void Unit::Select()
{
	CalculateWalkableTiles();
	HighlightWalkable();
}

void Unit::Move(std::vector<Node*> navPath)
{
	m_IsMoving = true;
	
	for (int i = 0; i < navPath.size(); i++)
	{
		Node* currentNode = navPath[i];
		Tile* tile = m_Tile->m_Game->GetTile(currentNode->m_XIndex, currentNode->m_YIndex);
		m_CurrentPath.push_back(tile);
	}

	m_HasMovedThisTurn = true;

}

void Unit::UpdateMove()
{
	if (m_IsMoving)
	{
		m_ElapsedMoveTime += m_Tile->m_Game->m_DeltaTime;

		if (m_ElapsedMoveTime >= 100)
		{
			m_ElapsedMoveTime = 0;
			if (m_CurrentPathIndex < m_CurrentPath.size() - 1)
			{
				m_CurrentPathIndex++;
				MoveTo(m_CurrentPathIndex);
			}
			else // We have reached our destination.
			{
				m_IsMoving = false;
				m_ElapsedMoveTime = 0;
				m_CurrentPath.clear();
				m_CurrentPathIndex = 0;
			}

		}
	}
}

void Unit::MoveTo(int pathIndex)
{
	m_Tile->ClearUnit();

	if (!m_HasSecondaryTile) // Don't need to set the tile back if we haven't replaced the tile.
		m_Tile->SetTile(m_Tile->m_OriginalType, true);
	else
		m_Tile->RemoveSecondaryTile();


	if (m_HasSecondaryTile)
		m_CurrentPath[pathIndex]->SetSecondaryTile(m_TileType);
	else
		m_CurrentPath[pathIndex]->SetTile(m_TileType, false);

	m_CurrentPath[pathIndex]->AttachUnit(this);

	m_Tile = m_CurrentPath[pathIndex];

	UpdateHealthBarPos();
}

bool Unit::IsNodeReachable(Node* node)
{
	for (int i = 0; i < m_WalkableTiles.size(); i++)
	{
		if (node == m_WalkableTiles[i]->m_Node)
			return true;
	}

	return false;
}

void Unit::HandleInput(SDL_Event& e)
{
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		int x, y;
		if (SDL_GetMouseState(&x, &y) && m_Tile->m_Game->m_CurrentlySelectedTile == m_Tile)
		{
			for (int i = 0; i < m_Buttons.size(); i++)
			{
				Button* currentButton = m_Buttons[i];
				if (x > currentButton->m_ActiveSprite->m_Rect->x && x < currentButton->m_ActiveSprite->m_Rect->x + currentButton->m_ActiveSprite->m_Rect->w)
				{
					if (y > currentButton->m_ActiveSprite->m_Rect->y && y < currentButton->m_ActiveSprite->m_Rect->y + currentButton->m_ActiveSprite->m_Rect->h)
					{
						std::cout << "Clicked a button." << std::endl;
						if (currentButton->m_Game->m_PlayerTurn == currentButton->m_Game->m_LocalPlayer->m_PlayerTurnNum && currentButton->m_Active) // Can only press buttons when it is our turn and if the
						{																															 // button is active.
							m_ProductionType = currentButton->m_prodType; // Pretty weird and bad way to get the button to produce the unit but watevs. We're relying
							currentButton->PerformAction(this);			  // on the units m_ProductionType not changing until the unit is produced. But I plan on having
																		  // villagers produce buildings instantly for now so this *should* be okay.
						}
					}
				}
			}
		}
	}
}

bool Unit::CheckCanPlace(TileType tileType)
{
	switch (m_ProductionType)
	{
	case PRODUCTION_TYPE::FARM:
		if (tileType == TileType::GRASS)
		{
			return true;
		}
		break;
	case PRODUCTION_TYPE::QUARRY:
		if (tileType == TileType::MOUNTAIN)
		{
			return true;
		}
		break;
	case PRODUCTION_TYPE::LUMBER:
		if (tileType == TileType::FOREST)
		{
			return true;
		}
		break;
	}
	// If it doesn't hit any of those then it will return false.
	return false;
}


bool Unit::CanAfford(TileType buildingType)
{
	int currentWood = m_Tile->m_Game->m_LocalPlayer->m_wood;
	int currentFood = m_Tile->m_Game->m_LocalPlayer->m_food;
	int currentStone = m_Tile->m_Game->m_LocalPlayer->m_stone;

	bool success = true;
	switch (buildingType)
	{
	case TileType::FARM:

		if (currentWood - FARM_WOOD_COST < 0)
			success = false;
		if (currentFood - FARM_FOOD_COST < 0)
			success = false;
		if (currentStone - FARM_STONE_COST < 0)
			success = false;
		break;

	case TileType::LUMBER:

		if (currentWood - LUMBER_WOOD_COST < 0)
			success = false;
		if (currentFood - LUMBER_FOOD_COST < 0)
			success = false;
		if (currentStone - LUMBER_STONE_COST < 0)
			success = false;
		break;

	case TileType::QUARRY:

		if (currentWood - QUARRY_WOOD_COST < 0)
			success = false;
		if (currentFood - QUARRY_FOOD_COST < 0)
			success = false;
		if (currentStone - QUARRY_STONE_COST < 0)
			success = false;
		break;
	}

	return success;
	
}

void Unit::GenerateResources()
{
	// We'll only bother generating resources if the unit is either a LUMBER, QUARRY or FARM. All
	// other units don't generate anything.

	switch (m_TileType)
	{
	case TileType::FARM:
		m_Tile->m_Game->AddResource(FARM_WOOD_GENERATION, FARM_FOOD_GENERATION, FARM_STONE_GENERATION);
		break;
	case TileType::LUMBER:
		m_Tile->m_Game->AddResource(LUMBER_WOOD_GENERATION, LUMBER_FOOD_GENERATION, LUMBER_STONE_GENERATION);
		break;
	case TileType::QUARRY:
		m_Tile->m_Game->AddResource(QUARRY_WOOD_GENERATION, QUARRY_FOOD_GENERATION, QUARRY_STONE_GENERATION);
		break;
	default:
		std::cout << "Passed in a m_TileType that has no generation stats! Please pass in either a FARM, LUMBER or QUARRY!" << std::endl;
		break;
	}
}

void Unit::TakeDamage(float damage)
{
	m_Health -= damage;
	if (m_Health < 0)
	{
		m_Health = 0;
		for (int i = 0; i < m_Tile->m_Game->m_AllUnits.size(); i++)
		{
			if (m_Tile->m_Game->m_AllUnits[i] == this)
			{
				m_Tile->m_Game->DestroyUnit(i);
				break;
			}
		}
	}
}

void Unit::DrawHealth()
{
	SDL_SetRenderDrawColor(m_Tile->m_Renderer, 255, 0, 0, 255);

	m_HealthBarRect.w = (m_Health / 100) * (1280 / 32); // 64 is the width of the entire tile.
	SDL_RenderFillRect(m_Tile->m_Renderer, &m_HealthBarRect);
}

void Unit::UpdateHealthBarPos()
{
	m_HealthBarRect.x = m_Tile->m_xPos;
	m_HealthBarRect.y = m_Tile->m_yPos + 20;
}

bool Unit::CanAttack(Tile* tile)
{
	for (int i = 0; i < m_AttackableTiles.size(); i++)
	{
		if (tile == m_AttackableTiles[i])
		{
			return true;
		}
	}
	return false;
}

void Unit::CalculateAttackTiles(float gScoreReach)
{
	// Clearing old attackable tiles.
	m_AttackableTiles.clear();

	std::vector<Node*> nodesUnderGScore = Dijkstra::GetNodesUnderGScore(gScoreReach, m_Tile->m_Node, true); // By ignoring passability we can attack units.

	for (int i = 0; i < nodesUnderGScore.size(); i++)
	{
		m_AttackableTiles.push_back(m_Tile->m_Game->GetTile(nodesUnderGScore[i]->m_XIndex, nodesUnderGScore[i]->m_YIndex));
	}

}

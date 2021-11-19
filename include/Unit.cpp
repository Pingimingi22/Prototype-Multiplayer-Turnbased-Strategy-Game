#include "Unit.h"
#include "Button.h"
#include "Grid.h"
#include "Game.h"

// =============== Defines of some general gameplay stuff =============== //
#define VILLAGER_PRODUCTION_TURNS 2 // Takes 2 turns to produce one villager.

// ====================================================================== //

void Unit::AddButton(Button button)
{
	Button* newButton = new Button(button.m_ActiveSprite, button.m_InactiveSprite, true, button.m_Game, button.m_Type, this); // Important last parameter.
																															  // this keyword means this unit
																															  // will be attached to the button.
	m_Buttons.push_back(newButton);
}

void Unit::DrawButtons()
{
	for (int i = 0; i < m_Buttons.size(); i++)
	{
		m_Buttons[i]->Draw();
	}
}

void Unit::StartProduction(PRODUCTION_TYPE type)
{
	m_ProductionType = type;
	m_IsProducing = true;
	m_ProductionTurnsElapsed = 0;

	// When you start producing something from this unit, all buttons for this unit will be disabled.
	ToggleButtons(false);
}

bool Unit::Produce(Tile& output)
{
	if (CheckProduction()) // We can try and produce this item.
	{
		Tile availableTile;
		// Check surrounding tiles and if atleast 1 spot is clear, produce item and return true;
		if (m_Tile->CheckSurroundingTiles(availableTile))
		{
			// We have atleast one spot to put the produced item.
			Unit* newUnit = new Unit(m_Tile->m_Game->GetTile(availableTile.m_Node->m_XIndex, availableTile.m_Node->m_YIndex), m_Owner, true, true, TileType::VILLAGER, availableTile.m_Node->m_XIndex, availableTile.m_Node->m_YIndex, 5, true);
			m_Tile->m_Game->PlaceUnit(newUnit);


			TileType tileType = Tile::ProductionTypeToTileType(m_ProductionType);

			
			//availableTile.SetSecondaryTile(tileType);

			m_ProductionTurnsElapsed = 0;
			m_IsProducing = false;

			// Turn on all the unit's buttons.
			ToggleButtons(true);

			output = availableTile;
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

void Unit::CalculateWalkableTiles()
{
	m_WalkableTiles.clear(); // Clear previous calculation.

	Node* currentNode = m_Tile->m_Node;

	unsigned int currentReachLevel = 0;

	std::vector<Node*> currentLevel;

	// Get the initial 8 tiles.
	for (int i = 0; i < currentNode->m_ConnectedNodes.size(); i++)
	{
		Node* nodeToAdd = currentNode->m_ConnectedNodes[i];

		Tile* tile = m_Tile->m_Game->GetTile(nodeToAdd->m_XIndex, nodeToAdd->m_YIndex);

		currentLevel.push_back(nodeToAdd);
		m_WalkableTiles.push_back(tile);
	}
	currentReachLevel++; // We increment reach level to 1.


	std::vector<Node*> temp; // This should contain the outer ring which represents the next level.
	while (currentReachLevel < m_Reach)
	{
		for (int i = 0; i < currentLevel.size(); i++)
		{
			currentNode = currentLevel[i];
			for (int j = 0; j < currentNode->m_ConnectedNodes.size(); j++)
			{
				temp.push_back(currentNode->m_ConnectedNodes[j]);
			}
		}
		
		currentLevel.clear();
		for (int i = 0; i < temp.size(); i++)
		{
			currentLevel.push_back(temp[i]);
			m_WalkableTiles.push_back(m_Tile->m_Game->GetTile(temp[i]->m_XIndex, temp[i]->m_YIndex));
		}
		temp.clear();

		currentReachLevel++;
	}



	


}

void Unit::HighlightWalkable()
{
	for (int i = 0; i < m_WalkableTiles.size(); i++)
	{
		Tile* currentTile = m_WalkableTiles[i];
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
		m_Tile->SetTile(m_Tile->m_OriginalType);
	else
		m_Tile->RemoveSecondaryTile();


	if (m_HasSecondaryTile)
		m_CurrentPath[pathIndex]->SetSecondaryTile(m_TileType);
	else
		m_CurrentPath[pathIndex]->SetTile(m_TileType);

	m_CurrentPath[pathIndex]->AttachUnit(this);

	m_Tile = m_CurrentPath[pathIndex];
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
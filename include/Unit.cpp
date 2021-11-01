#include "Unit.h"
#include "Button.h"

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
			
			TileType tileType = Tile::ProductionTypeToTileType(m_ProductionType);
			availableTile.SetSecondaryTile(tileType);

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
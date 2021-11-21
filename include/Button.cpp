#include "Button.h"
#include <iostream>
#include "Game.h"
#include "Unit.h"

Button::Button(std::string path, float xPos, float yPos, float width, float height, SDL_Renderer* renderer, bool active)
{
	m_ActiveSprite = new Sprite(path, xPos, yPos, width, height, renderer);
	m_Active = active;
	m_Renderer = renderer;
}

Button::Button(Sprite* activeSprite, Sprite* inactiveSprite, bool active, Game* game, BUTTON_TYPE type, Unit* unit, PRODUCTION_TYPE prodType)
{
	m_ActiveSprite = activeSprite;
	m_InactiveSprite = inactiveSprite;
	active = active;
	m_Renderer = m_ActiveSprite->m_Renderer;
	m_Game = game;

	m_Type = type;
	m_Unit = unit;

	m_prodType = prodType;
}

void Button::Draw()
{
	if (m_Game->m_PlayerTurn == m_Game->m_LocalPlayer->m_PlayerTurnNum && m_Active) // Only draw green button if it is our turn to end.
	{
		SDL_Renderer* testRenderer = m_ActiveSprite->m_Renderer;
		SDL_Renderer* testRenderer2 = m_Renderer;
		m_ActiveSprite->Draw(m_Renderer);
	}
	else
		m_InactiveSprite->Draw(m_Renderer);
}

void Button::HandleInput(SDL_Event& e)
{
	// Moved to Unit.cpp HandleInput() function.

	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		int x, y;
		if (SDL_GetMouseState(&x, &y))
		{
			if (x > m_ActiveSprite->m_Rect->x && x < m_ActiveSprite->m_Rect->x + m_ActiveSprite->m_Rect->w)
			{
				if (y > m_ActiveSprite->m_Rect->y && y < m_ActiveSprite->m_Rect->y + m_ActiveSprite->m_Rect->h)
				{
					std::cout << "Clicked a button." << std::endl;
					if(m_Game->m_PlayerTurn == m_Game->m_LocalPlayer->m_PlayerTurnNum && m_Active) // Can only press buttons when it is our turn and if the
						PerformAction();														   // button is active.
				}
			}
		}
	}
}

void Button::PerformAction(Unit* unitCalling)
{
	switch (m_Type)
	{
	case BUTTON_TYPE::END_TURN:
		std::cout << "Ending turn..." << std::endl;
		EndTurn();
		break;
	case BUTTON_TYPE::PRODUCE:
		std::cout << "Producing item." << std::endl;
		Produce(unitCalling);
		break;
	case BUTTON_TYPE::PLACE:
		std::cout << "Placing item." << std::endl;
		Place(unitCalling);
	default:
		std::cout << "Error, button PerformAction() could not find a matching BUTTON_TYPE." << std::endl;
		break;
	}
}

void Button::EndTurn()
{
	m_Active = !m_Active;
	if (m_Game->m_PlayerTurn == m_Game->m_LocalPlayer->m_PlayerTurnNum) // Only allow pressing this button if it is our turn.
		m_Game->EndTurn();
}

void Button::Produce(Unit* unitCalling)
{
	if (unitCalling)
	{
		unitCalling->StartProduction(unitCalling->m_ProductionType);
	}
	else
	{
		std::cout << "Button that was clicked tried to produce an item, but no unit is attached to this button!" << std::endl;
	}

}

void Button::Place(Unit* unitCalling)
{
	if (unitCalling)
	{
		unitCalling->m_ProductionType = m_prodType;
		m_Game->m_IsPlacing = true;
	}
	else
	{
		std::cout << "Button that was clicked tried to place an item, but no unit is attached to this button!" << std::endl;
	}
}
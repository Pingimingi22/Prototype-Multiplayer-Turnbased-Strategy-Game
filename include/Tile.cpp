#include "Tile.h"
#include <iostream>
#include "Game.h"
#include "Unit.h"
#include "Grid.h"

Tile::Tile() {}
Tile::Tile(TileType type, float xPos, float yPos, float width, float height, SDL_Renderer* renderer, Game* game, Grid* worldGrid)
{
	m_Game = game;
	m_Type = type;
	m_OriginalType = type;
	//std::string texturePath = GetTilePath(type);

	m_Renderer = renderer;

	SDL_Texture* texture = GetCachedTexture(type);
	m_Sprite = new Sprite(texture, xPos, yPos, width, height, renderer);
	//m_Sprite = new Sprite(texturePath, xPos, yPos, width, height, renderer);
	m_xPos = xPos;
	m_yPos = yPos;
	m_Width = width;
	m_Height = height;

	// Initialising secondary tile sprite.
	m_SpriteSecondary = new Sprite(NULL, xPos, yPos, width, height, renderer);

	m_WorldGrid = worldGrid;

}

/// <summary>
/// Gets the already loaded in texture to save us from creating more textures than what we need.
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
SDL_Texture* Tile::GetCachedTexture(TileType type)
{
	switch (type)
	{
	case TileType::GRASS:
		return m_Game->m_GrassTexture;
		break;
	case TileType::FOREST:
		return m_Game->m_ForestTexture;
		break;
	case TileType::MOUNTAIN:
		return m_Game->m_MountainTexture;
		break;
	case TileType::CASTLE:
		return m_Game->m_CastleTexture;
		break;
	case TileType::VILLAGER:
		return m_Game->m_VillagerTexture;
		break;
	default:
		int hi = 5;
		break;
	

	}
}

std::string Tile::GetTilePath(TileType type)
{
	switch (type)
	{
		case TileType::GRASS:
			return "images/grass-001.png";
		case TileType::MOUNTAIN:
			return "images/mountain-001.png";
		case TileType::FOREST:
			return "images/forest-001.png";
		case TileType::CASTLE:
			return "images/castle-neutral-001.png";
		case TileType::VILLAGER:
			return "images/villager-0001.png";
		default:
			return "images/farm-001.png";
			
	}
}

void Tile::Draw(SDL_Renderer* renderer)
{
	m_Sprite->Draw(renderer);

	if (m_SpriteSecondary)
		m_SpriteSecondary->Draw(renderer);

	if (m_IsHighlighted)
		Highlight();

	

	if (m_Game->m_CurrentlySelectedTile == this)
	{
		// If we are the currently selected tile, find out if we have any buttons attached to our unit and if we do, draw them to the screen.
		if (m_Unit != nullptr)
		{
			// If we have a unit, we can draw all of our buttons.
			for (int i = 0; i < m_Unit->m_Buttons.size(); i++)
			{
				// Only draw the button if it is our building/unit.
				if(m_Unit->m_Owner == m_Game->m_LocalPlayer)
					m_Unit->m_Buttons[i]->Draw();
			}
		}
	}
}

void Tile::Update()
{
	if (m_Unit)
		m_Unit->UpdateMove();
}


void Tile::HandleInput(SDL_Event& e)
{
	// Handle input for each button this tile's unit has.
	if (m_Unit)
	{
		for (int i = 0; i < m_Unit->m_Buttons.size(); i++)
		{
			m_Unit->m_Buttons[i]->HandleInput(e);
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		int x, y;
		if (SDL_GetMouseState(&x, &y))
		{
			if (x > m_xPos && x < m_xPos + m_Width)
			{
				if (y > m_yPos && y < m_yPos + m_Height)
				{
					m_Game->m_SelectTexture->m_Rect->x = m_xPos;
					m_Game->m_SelectTexture->m_Rect->y = m_yPos;
					std::cout << "Clicked a " << GetName() << " tile." << std::endl;
					m_Game->m_TileSelectText.SetText(GetName());

					// Unhighlight previously highlighted selection.
					if(m_Game->m_CurrentlySelectedTile && m_Game->m_CurrentlySelectedTile->m_Unit)
						m_Game->m_CurrentlySelectedTile->m_Unit->Unhighlight();


					if (m_Game->m_CurrentlySelectedTile)
					{
						Unit* tileUnit = m_Game->m_CurrentlySelectedTile->m_Unit;
						if (tileUnit)
						{
							if (m_Game->m_CurrentlySelectedTile->m_Unit->IsNodeReachable(m_Node))
							{
								tileUnit->Move(Dijkstra::GetShortestPath(tileUnit->m_Tile->m_Node, m_Node));

								// Deselecting tiles after moving a unit so you don't get stuck in moving the same unit forever.
								m_Game->m_CurrentlySelectedTile = nullptr;
							}
							else
								m_Game->m_CurrentlySelectedTile = this;
						}
						else
							// Set the game's currently selected tile to this tile.
							m_Game->m_CurrentlySelectedTile = this;
					}
					else
					{
						m_Game->m_CurrentlySelectedTile = this;
						if (m_Unit)
						{
							m_Unit->Select();
						}
					}

					



					

				}
			}
		}
	}
}

std::string Tile::GetName()
{
	switch (m_Type)
	{
	case TileType::GRASS:
		return "grass";
	case TileType::FOREST:
		return "forest";
	case TileType::MOUNTAIN:
		return "mountain";
	case TileType::CASTLE:
	{
		std::string ownerName = m_Unit->m_Owner->m_username;
		return ownerName + "'s " + "castle";
	}

	}
}

void Tile::SetTile(TileType type)
{
	m_Type = type;
	//m_Sprite->ResetTexture(GetTilePath(type));
	m_Sprite->ResetTexture(GetCachedTexture(type));
}
void Tile::SetSecondaryTile(TileType type)
{
	m_HasSecondaryTile = true;
	SDL_Texture* secondarySpriteTexture = GetCachedTexture(type);
	m_SpriteSecondary->ResetTexture(secondarySpriteTexture);

	m_Node->SetPassable(false);
}

void Tile::RemoveSecondaryTile()
{
	m_HasSecondaryTile = false;
}

void Tile::AttachUnit(Unit* unit)
{
	m_Unit = unit;
}
void Tile ::ClearUnit()
{
	m_Unit = nullptr;
	//SetTile(m_OriginalType);

}

bool Tile::CheckSurroundingTiles(Tile& output)
{
	if (m_Node)
	{
		// If this tile is attached to the NavMap (it should be), then we can check the connected nodes.
		if (m_Node->m_ConnectedNodes.size() > 0)
		{
			bool success = false;
			for (int i = 0; i < m_Node->m_ConnectedNodes.size(); i++) // We also have to check if the nodes are passable, since a tile is only available if it can
			{														  // have things move and be placed on it.
				if (m_Node->m_ConnectedNodes[i]->m_Passable)
				{
					int nodeXIndex = m_Node->m_ConnectedNodes[i]->m_XIndex;
					int nodeYIndex = m_Node->m_ConnectedNodes[i]->m_YIndex;

					//Tile* testTile = m_Game->m_WorldGrid->GetTile(nodeXIndex, nodeYIndex);
					output = *m_Game->m_WorldGrid->GetTile(nodeXIndex, nodeYIndex); // output is set to the corresponding tile.
					success = true;
				}
			}
			return success;
		}
		else
			return false;
	}
	std::cout << "Warning! A tile is missing a NavMap node!" << std::endl;
	return false;
} 

TileType Tile::ProductionTypeToTileType(PRODUCTION_TYPE type)
{
	switch (type)
	{
	case PRODUCTION_TYPE::VILLAGER:
		return TileType::VILLAGER;
	default:
		return TileType::VILLAGER;
	}
}

void Tile::SetHighlight(Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool toggle)
{
	m_HighlightColour.r = r;
	m_HighlightColour.g = g;
	m_HighlightColour.b = b;
	m_HighlightColour.a = a;
	m_IsHighlighted = true;
}

void Tile::Highlight()
{
	//SDL_RenderDrawRect(m_Renderer, m_Sprite->m_Rect);
	SDL_SetRenderDrawColor(m_Renderer, m_HighlightColour.r, m_HighlightColour.g, m_HighlightColour.b, m_HighlightColour.a);
	SDL_RenderFillRect(m_Renderer, m_Sprite->m_Rect);

	m_IsHighlighted = true;
}
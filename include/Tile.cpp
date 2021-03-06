#include "Tile.h"
#include <iostream>
#include "Game.h"
#include "Unit.h"
#include "Grid.h"

#include "PacketStructs.h"

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
	case TileType::QUARRY:
		return m_Game->m_QuarryTexture;
	case TileType::LUMBER:
		return m_Game->m_LumberTexture;
	case TileType::FARM:
		return m_Game->m_FarmTexture;
	default:
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
		case TileType::QUARRY:
			return "images/quarry-001.png";
		case TileType::FARM:
			return "images/farm-001.png";
		case TileType::LUMBER:
			return "images/lumber-mill-001.png";
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

	
	if (m_Unit && m_Unit->m_Health < 100)
	{
		m_Unit->DrawHealth(); // Draw health to any units that don't have max health currently.
	}

	if (m_Game->m_CurrentlySelectedTile == this)
	{
		// If we are the currently selected tile, find out if we have any buttons attached to our unit and if we do, draw them to the screen.
		if (m_Unit != nullptr)
		{

			// Drawing the health bars of the units.
			m_Unit->DrawHealth();

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
			//m_Unit->m_Buttons[i]->HandleInput(e);
			m_Unit->HandleInput(e);
		}
	}

	int x, y;

	// ----------------------------- Temporary Hover Tile Selection ----------------------------- //

	SDL_GetMouseState(&x, &y);
	m_IsHovering = false;

	if (m_Game->m_IsPlacing)
	{
		if ((float)x > m_xPos && (float)x < m_xPos + m_Width)
		{
			if ((float)y > m_yPos && (float)y < m_yPos + m_Height)
			{
				m_IsHovering = true;
				m_Game->m_TileSelectText.SetText(GetName());

				if (!m_HasTempChange && !m_Unit) // Don't want to draw over units.
				{
					m_HasTempChange = true;
					m_PrevPassableState = m_Node->m_Passable;
					m_TempTypeCache = m_Type;
					SetTile(ProductionTypeToTileType(m_Game->m_CurrentlySelectedTile->m_Unit->m_ProductionType), false);

					std::cout << "Hovered over tile and changed accordingly." << std::endl;	
				}
				if (e.type == SDL_MOUSEBUTTONDOWN && m_Game->m_CurrentlySelectedTile->m_Unit->CheckCanPlace(m_TempTypeCache) && !m_Unit && m_Game->m_CurrentlySelectedTile->m_Unit->IsNodeReachable(m_Node) && e.button.button == SDL_BUTTON_LEFT)
				{
					m_Game->m_CurrentlySelectedTile->m_Unit->Place(this);
					m_Game->m_CurrentlySelectedTile->m_Unit->Unhighlight();
					m_Game->m_CurrentlySelectedTile = nullptr;

					m_HasTempChange = false;
					m_IsHovering = false;
				}
				else if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					if (e.button.button == SDL_BUTTON_RIGHT)
					{
						std::cout << "Attempting to deselect placement of building." << std::endl;
						
						m_Game->m_IsPlacing = false;
						m_Game->m_CurrentlySelectedTile->m_Unit->Unhighlight();
						m_Game->m_CurrentlySelectedTile = nullptr;

						m_IsHovering = false;
						//m_HasTempChange = false;

						
					}
				}
			}
		}



		if (m_HasTempChange && !m_IsHovering)
		{
			SetTile(m_TempTypeCache, m_PrevPassableState);
			m_IsHovering = false;
			m_HasTempChange = false;

		}
	}

	

	// ------------------------------------------------------------------------------------------ //

	if (e.type == SDL_MOUSEBUTTONDOWN && !m_Game->m_IsPlacing && e.button.button == SDL_BUTTON_LEFT)
	{
		//int x, y;
		if (SDL_GetMouseState(&x, &y))
		{
			if (x > m_xPos && x < m_xPos + m_Width)
			{
				if (y > m_yPos && y < m_yPos + m_Height)
				{
					bool isAttacking = false;
					// Before we attempt to move the unit, we have to check if the user has attacked with a villager.
					if (m_Game->m_CurrentlySelectedTile && m_Game->m_CurrentlySelectedTile->m_Unit && m_Game->m_CurrentlySelectedTile->m_Unit->m_TileType == TileType::VILLAGER)
					{
						if (m_Unit)
						{
							// If we get here, it means the user has clicked on something while having a villager selected.
							// This means if they now click on an enemy unit that is < 1.5f GScore away, they will attack that unit.

							if (m_Game->m_LocalPlayer->m_PlayerTurnNum == m_Game->m_CurrentlySelectedTile->m_Unit->m_OwnerTurnID && m_Game->m_PlayerTurn == m_Game->m_LocalPlayer->m_PlayerTurnNum) // Making sure it's our turn and our unit we are attacking with.
							{

								m_Game->m_CurrentlySelectedTile->m_Unit->CalculateAttackTiles(1.5f);


								if (m_Game->m_CurrentlySelectedTile->m_Unit->CanAttack(this) && m_Unit->m_OwnerTurnID != m_Game->m_PlayerTurn && !m_Game->m_CurrentlySelectedTile->m_Unit->m_HasAttackedThisTurn) // Prevents attacking our own units.
								{
									m_Game->m_CurrentlySelectedTile->m_Unit->m_HasAttackedThisTurn = true;

									// This means they have clicked on an enemy unit with a villager.
									std::cout << "Attacked unit!" << std::endl;
									isAttacking = true;
									m_Unit->TakeDamage(25);

									// Tell other clients that we have taken damage.
									UnitDamagePacket damagePack;
									damagePack.damage = 25;
									damagePack.playerTurnID = m_Game->m_PlayerTurn;
									damagePack.tileOriginalPos = { m_Node->m_XIndex, m_Node->m_YIndex };

									m_Game->m_LocalPlayer->peer->Send((char*)&damagePack, sizeof(damagePack), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);

								}
							}
						}
					}
					if(!isAttacking)
					{

						m_Game->m_SelectTexture->m_Rect->x = m_xPos;
						m_Game->m_SelectTexture->m_Rect->y = m_yPos;
						std::cout << "Clicked a " << GetName() << " tile." << std::endl;
						m_Game->m_TileSelectText.SetText(GetName());

						// Unhighlight previously highlighted selection.
						if (m_Game->m_CurrentlySelectedTile && m_Game->m_CurrentlySelectedTile->m_Unit)
							m_Game->m_CurrentlySelectedTile->m_Unit->Unhighlight();



						// To track whether or not we moved a unit with this selection.
						bool movedUnit = false;
						bool hasSelectedSameUnit = false;
						// If we already had a selected tile.
						if (m_Game->m_CurrentlySelectedTile)
						{
							// If the previously selected tile was a unit, we want to treat this selection as a move order.
							Unit* tileUnit = m_Game->m_CurrentlySelectedTile->m_Unit;
							if (tileUnit)
							{
								if (!tileUnit->m_HasMovedThisTurn && m_Game->m_PlayerTurn == m_Game->m_LocalPlayer->m_PlayerTurnNum && m_Game->m_CurrentlySelectedTile->m_Unit->m_OwnerTurnID == m_Game->m_LocalPlayer->m_PlayerTurnNum) // Can only move unit once
								{																																																		 // and if it is our turn.

									if (tileUnit == m_Unit)
									{
										// We clicked on the same unit that we are, so we should deselect ourselves.
										m_Game->m_CurrentlySelectedTile = nullptr;
										hasSelectedSameUnit = true;
									}

									// Only attempt to move the unit if the passed in node is reachable.
									else if (m_Game->m_CurrentlySelectedTile->m_Unit->IsNodeReachable(m_Node))
									{
										tileUnit->Move(Dijkstra::GetShortestPath(tileUnit->m_Tile->m_Node, m_Node));

										// Send move packet to all other players.
										m_Game->SendUnitMove({ (int)m_Node->m_XIndex , (int)m_Node->m_YIndex }, tileUnit);

										// Deselecting tiles after moving a unit so you don't get stuck in moving the same unit forever.
										m_Game->m_CurrentlySelectedTile = nullptr;

										movedUnit = true;
										hasSelectedSameUnit = false;
									}
								}

								// This extra else if is required because it allows us to deselect units after they have moved or if it is not our turn.
								else if (tileUnit == m_Unit)
								{
									// We clicked on the same unit that we are, so we should deselect ourselves.
									m_Game->m_CurrentlySelectedTile = nullptr;
									hasSelectedSameUnit = true;

								}

							}
						}
						if (!movedUnit && !hasSelectedSameUnit)
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
	case TileType::LUMBER:
		return "lumber-mill";
	case TileType::QUARRY:
		return "quarry";
	case TileType::FARM:
		return "farm";
	default:
		return "no-name for tile.";

	}
}

void Tile::SetTile(TileType type, bool passable)
{
	m_Type = type;
	//m_Sprite->ResetTexture(GetTilePath(type));
	m_Sprite->ResetTexture(GetCachedTexture(type));

	m_Node->SetPassable(passable);
}
void Tile::SetSecondaryTile(TileType type)
{
	m_HasSecondaryTile = true;
	SDL_Texture* secondarySpriteTexture = GetCachedTexture(type);

	if(m_SpriteSecondary)
		m_SpriteSecondary->ResetTexture(secondarySpriteTexture);


	m_Node->SetPassable(false);
}

void Tile::RemoveSecondaryTile()
{
	m_HasSecondaryTile = false;
	m_SpriteSecondary->m_Texture = nullptr;
	//m_SpriteSecondary = nullptr;
	m_Node->SetPassable(true);
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
	case PRODUCTION_TYPE::FARM:
		return TileType::FARM;
	case PRODUCTION_TYPE::LUMBER:
		return TileType::LUMBER;
	case PRODUCTION_TYPE::QUARRY:
		return TileType::QUARRY;
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
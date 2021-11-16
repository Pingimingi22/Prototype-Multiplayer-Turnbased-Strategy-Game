#include "Grid.h"
#include <iostream>
#include "Game.h"
#include "NavMap.h"
Grid::Grid(float width, float height, float xSpacing, float ySpacing, SDL_Renderer* renderer, Game* game, NavMap* navMap)
{
	m_Width = width;
	m_Height = height;

	m_Renderer = renderer;

	m_Game = game;

	for (int i = 0; i < width; i++)
	{
		std::vector<Tile*> tileVec;
		m_TileMap.push_back(tileVec);
		for (int j = 0; j < height; j++)
		{
			Tile* tileYVec;
			m_TileMap[i].push_back(tileYVec);
		}
	}

	m_NavMap = navMap;
}

void Grid::Init(const Tmx::TileLayer* tileLayer)
{
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			float tileWidth = 1280 / 32;
			float tileHeight = 800 / 32;
			TileType newType = (TileType)tileLayer->GetTileId(i, j);
			if (newType == 0)
			{
				std::cout << "Broken" << std::endl;
			}
			Tile* newTile = new Tile(newType, i * tileWidth, j * tileHeight, tileWidth, tileHeight, m_Renderer, m_Game, this);
			m_TileMap[i][j] = newTile;
			m_TileMap[i][j]->m_Node = m_NavMap->m_Grid[i][j];
			
			// Making mountain tiles inpassable.
			if (m_TileMap[i][j]->m_Type == TileType::MOUNTAIN)
				m_TileMap[i][j]->m_Node->SetPassable(false);
		}
	}
}

void Grid::Draw(SDL_Renderer* renderer)
{
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_TileMap[i][j]->Draw(m_Renderer);
		}
	}
}

void Grid::Update()
{
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_TileMap[i][j]->Update();
		}
	}
}

void Grid::HandleInput(SDL_Event& e)
{
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_TileMap[i][j]->HandleInput(e);
		}
	}
}

Tile* Grid::GetTile(int x, int y)
{
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			if(i == x && j == y)
				return m_TileMap[i][j];
		}
	}
}
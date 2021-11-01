#include "Sprite.h"
#include "SDL_image.h"
#include <iostream>


Sprite::Sprite(std::string path, float x, float y, float width, float height, SDL_Renderer* renderer)
{
	m_Rect = new SDL_Rect();
	m_Rect->x = x;
	m_Rect->y = y;
	m_Rect->w = width;
	m_Rect->h = height;

	m_Renderer = renderer;
	InitTexture(path, renderer);
}

Sprite::Sprite(SDL_Texture* texture, float x, float y, float width, float height, SDL_Renderer* renderer)
{
	m_Rect = new SDL_Rect();
	m_Rect->x = x;
	m_Rect->y = y;
	m_Rect->w = width;
	m_Rect->h = height;

	m_Renderer = renderer;
	m_Texture = texture;
}

void Sprite::SetPos(float x, float y)
{
	m_Rect->x = x;
	m_Rect->y = y;
}

void Sprite::Draw(SDL_Renderer* renderer)
{
	//SDL_RenderCopy(m_Renderer, m_Texture, NULL, m_Rect);
	SDL_RenderCopy(renderer, m_Texture, NULL, m_Rect);
}

void Sprite::InitTexture(std::string path, SDL_Renderer* renderer)
{
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
		std::cout << "Error loading image." << std::endl;
	else
	{
		// Loading the image worked.
		m_Texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (m_Texture == NULL)
		{
			std::cout << "Error calling SDL_CreateTextureFromSurface!" << std::endl;
		}
		else
		{
			SDL_FreeSurface(loadedSurface);
		}
	}
}

void Sprite::ResetTexture(std::string path)
{
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
		std::cout << "Error loading image." << std::endl;
	else
	{
		// Loading the image worked.

		SDL_DestroyTexture(m_Texture); // Don't destroy texture! The sprite might not own it!
		m_Texture = NULL;
		

		m_Texture = SDL_CreateTextureFromSurface(m_Renderer, loadedSurface);
		if (m_Texture == NULL)
		{
			std::cout << "Error calling SDL_CreateTextureFromSurface!" << std::endl;
		}
	}
}

void Sprite::ResetTexture(SDL_Texture* newTexture)
{
	m_Texture = newTexture;
}

#include "Text.h"
#include <iostream>

Text::Text(std::string text, SDL_Color colour, TTF_Font* font, SDL_Renderer* renderer, float xPos, float yPos, float height)
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), colour);

	m_Rect = new SDL_Rect;
	m_Font = font;
	m_Colour = colour;
	m_Renderer = renderer;

	if (textSurface == NULL)
	{
		std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
	else
	{
		// loading the text surface worked.
		m_Texture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (m_Texture == NULL)
		{
			std::cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
		}
		else
		{
			// Creating texture from text worked.
			m_Width = textSurface->w;
			m_Height = textSurface->h;

			m_Rect->w = 125;
			m_Rect->h = height;//45;
			m_Rect->x = xPos;//50;
			m_Rect->y = yPos;//825;
		}

		SDL_FreeSurface(textSurface);
	}
}

void Text::SetText(std::string text)
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(m_Font, text.c_str(), m_Colour);
	if (textSurface == NULL)
	{
		std::cout << "SetText() failed to render text surface." << std::endl;
	}
	else
	{
		// It worked.
		SDL_DestroyTexture(m_Texture);
		m_Texture = NULL;
		
		m_Texture = SDL_CreateTextureFromSurface(m_Renderer, textSurface);

		if (m_Texture == NULL)
		{
			std::cout << "SetText() failed to create texture from surface!" << std::endl;

		}

		m_Rect->w = text.size() * 10;
	}

	SDL_FreeSurface(textSurface);

}

void Text::Draw(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, m_Texture, NULL, m_Rect);
}
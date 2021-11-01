#pragma once
#include "SDL.h"
#include <string>
#include "SDL_ttf.h"

class Text
{
public:

	SDL_Texture* m_Texture;
	SDL_Rect* m_Rect;
	std::string m_Text;
	float m_Width;
	float m_Height;
	TTF_Font* m_Font;
	SDL_Color m_Colour;
	SDL_Renderer* m_Renderer;
	

	Text(std::string text, SDL_Color colour, TTF_Font* font, SDL_Renderer* renderer);
	Text() {}

	void SetText(std::string text);

	void Draw(SDL_Renderer* renderer);

};
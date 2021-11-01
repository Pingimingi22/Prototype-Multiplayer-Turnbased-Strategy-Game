#pragma once
#include "SDL.h"
#include <string>
class Sprite
{
public:
	SDL_Rect* m_Rect;
	SDL_Texture* m_Texture;
	SDL_Renderer* m_Renderer;

	Sprite(std::string path, float x, float y, float width, float height, SDL_Renderer* renderer);
	Sprite(SDL_Texture* texture, float x, float y, float width, float height, SDL_Renderer* renderer);

	//Sprite(const Sprite& copy)
	//{
	//	m_Rect = copy.m_Rect;
	//	m_Texture = copy.m_Texture;
	//	m_Renderer = copy.m_Renderer;
	//}

	void SetPos(float x, float y);
	void Draw(SDL_Renderer* renderer);

	void ResetTexture(std::string path);
	void ResetTexture(SDL_Texture* newTexture);

private:
	void InitTexture(std::string path, SDL_Renderer* renderer);

};
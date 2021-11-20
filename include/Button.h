#pragma once
#include "Sprite.h"


enum class BUTTON_TYPE
{
	END_TURN,
	PRODUCE
};

class Unit;
class Game;
class Button
{
public:
	Sprite* m_ActiveSprite;
	Sprite* m_InactiveSprite;
	bool m_Active;
	SDL_Renderer* m_Renderer;
	Game* m_Game;

	// Type of this button.
	BUTTON_TYPE m_Type;

	// Stuff for buttons that produce items.
	// These types of buttons need a reference to the unit that controls them, so that they know what item to produce.
	Unit* m_Unit;

	Button(std::string path, float xPos, float yPos, float width, float height, SDL_Renderer* renderer, bool active);
	Button(Sprite* activeSprite, Sprite* inactiveSprite, bool active, Game* game, BUTTON_TYPE type, Unit* = nullptr);

	void Draw();

	void HandleInput(SDL_Event& e);

	void PerformAction(Unit* unitCalling = nullptr);

private:
	void EndTurn();
	void Produce(Unit* unitCalling);

};
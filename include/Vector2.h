#pragma once
class Vector2
{
public:
	float x;
	float y;

	Vector2() {}
	Vector2(float xPos, float yPos) { x = xPos; y = yPos; }
	Vector2(int xPos, int yPos) { x = xPos; y = yPos; }

	bool operator==(const Vector2& other)
	{
		return (x == other.x && y == other.y);
	}


};
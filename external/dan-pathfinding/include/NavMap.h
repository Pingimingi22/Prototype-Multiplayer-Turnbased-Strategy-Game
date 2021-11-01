#pragma once
#include "Node.h"
#include <vector>

/// <summary>
/// NavMap is the class responsible for holding the navigation graph. All it contains is a 2D std::vector which represents the world.
/// References to the nodes in this grid can be passed to your actual game world grid.
/// </summary>

class NavMap
{
public:

	std::vector<std::vector<Node*>> m_Grid;
	int m_Width;
	int m_Height;

	NavMap(int width, int height);

	void ConnectNodes();

};
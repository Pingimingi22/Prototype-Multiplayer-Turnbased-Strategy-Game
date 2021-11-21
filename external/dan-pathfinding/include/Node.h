#pragma once
#include <vector>


class Node
{
public:
	std::vector<Node*> m_ConnectedNodes;

	// Reference to position in the 2D std::vector.
	int m_XIndex = 0;
	int m_YIndex = 0;

	bool m_Passable = true;


	// Dijkstra internal searching stuff.
	Node* m_Previous;
	float m_Cost;
	float m_GScore;

	Node();
	Node(int x, int y);
	void ResetGScore(); // Helper function for pathfinding.
	void ResetPrevious(); // helper function for pathfinding.
	bool Compare(Node* otherNode);
	void SetPassable(bool toggle);

	bool CheckIfConnected(Node* node);
	

};
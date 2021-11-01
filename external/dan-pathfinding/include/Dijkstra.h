#pragma once
#include "Node.h"

/// <summary>
/// Made by Daniel Hadzajlic.
/// 
/// This is my version of the Dijkstra Shortest Path algorithm.
/// </summary>
static class Dijkstra
{
public:
	static std::vector<Node*> GetShortestPath(Node* start, Node* destination);
	static bool CheckIfInList(Node* node, std::vector<Node*> list);

};
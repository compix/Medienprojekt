#pragma once
#include <stdint.h>

enum class NodeState
{
	UNVISITED,
	OPEN,
	CLOSED
};

struct NodePathInfo
{
	NodePathInfo() : state(NodeState::UNVISITED), cost(0), costSoFar(0), estimatedTotalCost(0), prevOnPath(nullptr), next(nullptr), prev(nullptr) {}

	/**
	* Inserts itself at the correct position starting at the given node to remain the cost order: lowest -> highest
	* This is fast because it's used in a grid and the costs are usually uniform. The open list is small. Updates/relaxations are rarely needed.
	* Runtime: Usually O(1) - Theoretical worst case: O(n) where n is the number of nodes in the open list.
	*/
	void insert(NodePathInfo* at);

	/**
	* Removes itself from the list.
	* Runtime: O(1)
	*/
	NodePathInfo* remove();

	uint8_t x, y;
	NodeState state;
	uint32_t cost;
	uint32_t costSoFar;
	uint32_t estimatedTotalCost;
	NodePathInfo* prevOnPath;

	// List information to keep a sorted list.
	NodePathInfo* next;
	NodePathInfo* prev;
};
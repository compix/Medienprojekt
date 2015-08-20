#pragma once
#include <vector>
#include "GraphNode.h"

struct AIPath
{
	AIPath() : cost(0), rating(-FLT_MAX), curNode(0) {}

	void attach(AIPath& path);

	inline bool reachedGoal() { return curNode == nodes.size() - 1; }
	inline GraphNode* goal() { return nodes.size() > 0 ? nodes[nodes.size() - 1] : nullptr; }
	inline GraphNode* first() { return nodes.size() > 0 ? nodes[0] : nullptr; }
	inline void resetRating() { rating = -FLT_MAX; }

	std::vector<GraphNode*> nodes;
	uint16_t cost;
	uint16_t curNode;

	// How good is the path? Higher values = better path
	float rating;
};
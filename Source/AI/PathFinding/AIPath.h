#pragma once
#include <vector>
#include "GraphNode.h"

struct AIPath
{
	AIPath() : cost(0), rating(-FLT_MAX) {}

	inline void attach(AIPath& path)
	{
		for (int i = 0; i < nodes.size(); ++i)
			nodes.push_back(path.nodes[i]);
		cost += path.cost;
	}

	std::vector<GraphNode*> nodes;
	uint16_t cost;

	// How good is the path? Higher values = better path
	float rating;
};
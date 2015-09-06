#pragma once
#include <vector>
#include <float.h>
#include <iostream>
#include "../GraphNode.h"
#include "../AIUtil.h"

struct AIPath
{
    AIPath() : cost(0), curNode(0), behaviorNode(nullptr), rating(-FLT_MAX) {}

	void attach(AIPath& path);

	inline bool reachedGoal() { return curNode == nodes.size() - 1; }
	inline GraphNode* goal() { return nodes.size() > 0 ? nodes[nodes.size() - 1] : nullptr; }
	inline GraphNode* first() { return nodes.size() > 0 ? nodes[0] : nullptr; }
	inline GraphNode* current() { return curNode < nodes.size() ? nodes[curNode] : nullptr; }
	inline void resetRating() { rating = -FLT_MAX; }

	std::vector<GraphNode*> nodes;
	uint16_t cost;
	uint16_t curNode;
	GraphNode* behaviorNode; // Used for some behavior like punch.

	// How good is the path? Higher values = better path
	float rating;

	bool operator==(const AIPath& other);

	inline float requiredTime(entityx::Entity& entity) const { return nodes.empty() ? 0 : (nodes.size() - 1) * AIUtil::getTimePerCell(entity); }
};

std::ostream& operator<<(std::ostream& stream, const AIPath& path);

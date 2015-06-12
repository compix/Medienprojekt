#pragma once
#include <stdint.h>


// Experimental node costs
namespace NodeCost
{
	const uint32_t NORMAL = 100;

	const uint32_t DANGER_LOW = 300;
	const uint32_t DANGER_NORMAL = 400;
	const uint32_t DANGER_HIGH = 10000;
}

struct GraphNode
{
	GraphNode() : cost(NodeCost::NORMAL), valid(false) { }

	uint8_t x, y;
	uint32_t cost;
	bool valid;

	// Path Information
	GraphNode* prevOnPath;

	uint32_t costSoFar;
	uint32_t estimatedTotalCost;

	enum State
	{
		UNVISITED,
		OPEN,
		CLOSED
	} state;

	// List information to keep a sorted list.
	GraphNode* next;
	GraphNode* prev;
};
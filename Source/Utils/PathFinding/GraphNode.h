#pragma once
#include <stdint.h>

struct GraphNode
{
	enum Neighbor
	{
		LEFT,
		RIGHT,
		TOP,
		BOT
	};

	GraphNode() : cost(1.f), valid(false) { }

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
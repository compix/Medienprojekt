#pragma once
#include <stdint.h>

struct GraphNode
{
	GraphNode() : cost(1.f)
	{
		for (int i = 0; i < 4; ++i)
			neighbors[i] = nullptr;
	}

	GraphNode* neighbors[4];
	uint16_t id;
	float cost;
};
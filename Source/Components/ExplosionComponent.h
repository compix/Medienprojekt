#pragma once
#include "../Utils/Common.h"


struct SpreadComponent
{
	SpreadComponent(Direction direction, int range, float spreadTime) 
		: direction(direction), range(range), spreadTime(spreadTime), timeTillNext(spreadTime), stopped(false) {}

	Direction direction;
	int range; // how far it can spread in cells
	float spreadTime; // in seconds
	float timeTillNext; // in seconds
	bool stopped;
};

/**
 * @brief	Just a flag.
 */
struct ExplosionComponent {};
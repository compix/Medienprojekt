#pragma once
#include "../Utils/Common.h"


struct SpreadComponent
{
	SpreadComponent(Direction direction, int range, float spreadTime, bool ghost) 
		: direction(direction), range(range), spreadTime(spreadTime), timeTillNext(spreadTime), stopped(false), ghost(ghost) {}

	Direction direction;
	int range; // how far it can spread in cells
	float spreadTime; // in seconds
	float timeTillNext; // in seconds
	bool stopped;
	bool ghost; // Explosion will only be stopped by solid blocks
};

/**
 * @brief	Just a flag.
 */
struct ExplosionComponent {};
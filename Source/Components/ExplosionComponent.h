#pragma once
#include "../Utils/Common.h"


struct SpreadComponent
{
	SpreadComponent(Direction direction, int range, float spreadTime, bool ghost, bool lightning) 
		: direction(direction), range(range), spreadTime(spreadTime), timeTillNext(spreadTime), stopped(false), ghost(ghost), lightning(lightning)  {}

	Direction direction;
	int range; // how far it can spread in cells
	float spreadTime; // in seconds
	float timeTillNext; // in seconds
	bool stopped;
	bool ghost; // Explosion will only be stopped by solid blocks
	bool lightning; // 1 range explosion when it is stopped
};

/**
 * @brief	Just a flag.
 */
struct ExplosionComponent {};
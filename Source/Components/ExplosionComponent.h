#pragma once
#include <ecstasy/core/Component.h>
#include "../Utils/Common.h"

struct SpreadComponent: public Component<SpreadComponent>
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
struct ExplosionComponent: public Component<ExplosionComponent> {};
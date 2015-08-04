#pragma once
#include <ecstasy/core/Component.h>
#include "../Utils/Common.h"


struct SpreadComponent: public Component<SpreadComponent>
{
	Direction direction = Direction::DOWN;
	int range = 0; // how far it can spread in cells
	float spreadTime = 0; // in seconds
	float timeTillNext = 0; // in seconds
	bool stopped = false;
	
	void reset() override {
		direction = Direction::DOWN;
		range = 0;
		spreadTime = 0;
		timeTillNext = 0;
		stopped = false;
	}
};

/**
 * @brief	Just a flag.
 */
struct ExplosionComponent: public Component<ExplosionComponent> {};
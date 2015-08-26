#pragma once
#include "../Utils/Common.h"

enum class BombType : uint8_t;

struct SpreadComponent
{
	SpreadComponent(Direction direction, int range, float spreadTime, BombType bombType)
		: direction(direction), range(range), spreadTime(spreadTime), timeTillNext(spreadTime), stopped(false), bombType(bombType)  {}

	Direction direction;
	int range; // how far it can spread in cells
	float spreadTime; // in seconds
	float timeTillNext; // in seconds
	bool stopped;
	BombType bombType;
};

/**
 * @brief	Just a flag.
 */
struct ExplosionComponent {};
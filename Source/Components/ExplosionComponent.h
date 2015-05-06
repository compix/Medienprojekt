#pragma once

namespace ExplosionDirection
{
	enum Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};
}

/**
 * @brief	The explosion that spreads in a direction.
 */
struct ExplosionComponent
{
	ExplosionComponent(ExplosionDirection::Direction direction, int range, float spreadTime) : direction(direction), range(range), spreadTime(spreadTime), timeTillNext(spreadTime) {}

	ExplosionDirection::Direction direction;
	int range; // how far it can spread in cells
	float spreadTime; // in seconds
	float timeTillNext; // in seconds
};
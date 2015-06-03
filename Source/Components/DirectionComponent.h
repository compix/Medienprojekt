#pragma once
#include "../Utils/Common.h"

struct DirectionComponent
{
	DirectionComponent() : direction(Direction::DOWN) {}
	DirectionComponent(Direction dir) : direction(dir) {}

	Direction direction;
};
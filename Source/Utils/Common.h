#pragma once
#include<stdint.h>

enum class Direction : uint8_t
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

namespace CommonUtil
{
	Direction toDirection(int dirX, int dirY);
}

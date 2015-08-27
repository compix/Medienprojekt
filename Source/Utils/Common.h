#pragma once
#include<stdint.h>
#include<stddef.h>

enum class Direction : uint8_t
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct LevelCell
{
	uint8_t x;
	uint8_t y;
};

namespace CommonUtil
{
	Direction toDirection(int dirX, int dirY);
}

struct EnumClassHash
{
	template <typename T>
	size_t operator()(T t) const
	{
		return static_cast<size_t>(t);
	}
};

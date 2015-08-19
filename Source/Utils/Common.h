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

struct EnumClassHash
{
	template <typename T>
	size_t operator()(T t) const
	{
		return static_cast<size_t>(t);
	}
};

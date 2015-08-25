#include "Common.h"
#include <assert.h>

Direction CommonUtil::toDirection(int dirX, int dirY)
{
	assert(dirX == 0 || dirY == 0);

	switch (dirX)
	{
	case 1:
		return Direction::RIGHT;
	case -1:
		return Direction::LEFT;
	default:
		switch (dirY)
		{
		case 1:
			return Direction::DOWN;
		case -1:
			return Direction::UP;
		}
	}

	// This should never happen -> obviously wrong input
	assert(false);
	return Direction::DOWN;
}
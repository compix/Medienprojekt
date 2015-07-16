#pragma once
#include <cinttypes>

struct PlayerComponent
{
	PlayerComponent(uint8_t index)
		: index(index) {}

	uint8_t index;
};
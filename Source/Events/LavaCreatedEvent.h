#pragma once
#include <cinttypes>

struct LavaCreatedEvent
{
	LavaCreatedEvent(uint8_t cellX, uint8_t cellY) : cellX(cellX), cellY(cellY) {}

	uint8_t cellX;
	uint8_t cellY;
};
#pragma once
#include <cinttypes>

struct LavaSpotMarkedEvent
{
	LavaSpotMarkedEvent(uint8_t cellX, uint8_t cellY) : cellX(cellX), cellY(cellY) {}

	uint8_t cellX;
	uint8_t cellY;
};
#pragma once
#include <entityx/Entity.h>
#include <cinttypes>

struct BombLandedOnEntityEvent
{
	BombLandedOnEntityEvent(entityx::Entity entity, uint8_t cellX, uint8_t cellY)
		:entity(entity), cellX(cellX), cellY(cellY) {}

	entityx::Entity entity;
	uint8_t cellX;
	uint8_t cellY;
};
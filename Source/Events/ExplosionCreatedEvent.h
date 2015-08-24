#pragma once
#include <entityx/Entity.h>
#include "../Utils/Common.h"

using entityx::Entity;
struct ExplosionCreatedEvent
{
	ExplosionCreatedEvent(Entity entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime, bool ghost)
		: entity(entity), x(x), y(y), direction(direction), range(range), spreadTime(spreadTime), ghost(ghost) {}

	Entity entity;
	uint8_t x;
	uint8_t y;
	Direction direction;
	uint8_t range;
	float spreadTime;
	bool ghost;
};
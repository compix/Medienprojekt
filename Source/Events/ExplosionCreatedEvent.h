#pragma once
#include <entityx/Entity.h>
#include "../Utils/Common.h"

using entityx::Entity;
struct ExplosionCreatedEvent
{
	ExplosionCreatedEvent(Entity entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime, bool ghost, bool lightning, bool lightningPeak)
		: entity(entity), x(x), y(y), direction(direction), range(range), spreadTime(spreadTime), ghost(ghost), lightning(lightning), lightningPeak(lightningPeak) {}

	Entity entity;
	uint8_t x;
	uint8_t y;
	Direction direction;
	uint8_t range;
	float spreadTime;
	bool ghost;
	bool lightning;
	bool lightningPeak;
};
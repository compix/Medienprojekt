#pragma once
#include <entityx/Entity.h>
#include "../Utils/Common.h"

using entityx::Entity;
enum class BombType : uint8_t;

struct ExplosionCreatedEvent
{
	ExplosionCreatedEvent(Entity entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime, BombType bombType)
		: entity(entity), x(x), y(y), direction(direction), range(range), spreadTime(spreadTime), bombType(bombType) {}

	Entity entity;
	uint8_t x;
	uint8_t y;
	Direction direction;
	uint8_t range;
	float spreadTime;
	BombType bombType;
};
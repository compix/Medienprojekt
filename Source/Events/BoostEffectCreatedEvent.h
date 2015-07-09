#pragma once
#include <entityx/Entity.h>

using entityx::Entity;
struct BoostEffectCreatedEvent
{
	BoostEffectCreatedEvent(Entity entity, uint8_t x, uint8_t y, Entity target)
		: entity(entity), x(x), y(y), target(target) {}

	Entity entity;
	Entity target;
	uint8_t x;
	uint8_t y;
};
#pragma once
#include <entityx/Entity.h>

using entityx::Entity;
struct BoostEffectCreatedEvent
{
	BoostEffectCreatedEvent(Entity entity, uint8_t x, uint8_t y, Entity target)
        : entity(entity), target(target), x(x), y(y) {}

	Entity entity;
	Entity target;
	uint8_t x;
	uint8_t y;
};

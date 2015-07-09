#pragma once
#include <entityx/Entity.h>
#include "../Components/ItemComponent.h"

using entityx::Entity;
struct SmokeCreatedEvent
{
	SmokeCreatedEvent(Entity entity, uint8_t x, uint8_t y)
		: entity(entity), x(x), y(y) {}

	Entity entity;
	uint8_t x;
	uint8_t y;
};
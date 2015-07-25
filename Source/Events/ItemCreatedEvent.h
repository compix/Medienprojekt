#pragma once
#include <entityx/Entity.h>
#include "../Components/ItemComponent.h"

using entityx::Entity;
struct ItemCreatedEvent
{
	ItemCreatedEvent(Entity entity, uint8_t x, uint8_t y, ItemType type)
		: entity(entity), x(x), y(y), type(type) {}

	Entity entity;
	ItemType type;
	uint8_t x;
	uint8_t y;
};
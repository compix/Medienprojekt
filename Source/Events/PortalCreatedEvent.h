#pragma once
#include <entityx/Entity.h>

using entityx::Entity;
struct PortalCreatedEvent
{
	PortalCreatedEvent(Entity entity, uint8_t x, uint8_t y, Entity owner)
        : entity(entity), owner(owner), x(x), y(y) {}

	Entity entity;
	Entity owner;
	uint8_t x;
	uint8_t y;
};

#pragma once
#include <entityx/Entity.h>

using entityx::Entity;
struct BombCreatedEvent
{
	BombCreatedEvent(Entity entity, uint8_t x, uint8_t y, Entity owner, bool ghost)
		: entity(entity), x(x), y(y), owner(owner), ghost(ghost) {}

	Entity entity;
	Entity owner;
	uint8_t x;
	uint8_t y;
	bool ghost;
};
#pragma once
#include <entityx/Entity.h>

using entityx::Entity;
struct BombCreatedEvent
{
	BombCreatedEvent(Entity entity, uint8_t x, uint8_t y, Entity owner, bool ghost, bool lightning)
		: entity(entity), x(x), y(y), owner(owner), ghost(ghost), lightning(lightning) {}

	Entity entity;
	Entity owner;
	uint8_t x;
	uint8_t y;
	bool ghost;
	bool lightning;
};
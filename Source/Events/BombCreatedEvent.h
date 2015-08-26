#pragma once
#include <entityx/Entity.h>

using entityx::Entity;
struct BombCreatedEvent
{
	BombCreatedEvent(Entity entity, uint8_t x, uint8_t y, Entity owner, BombType type)
		: entity(entity), x(x), y(y), owner(owner), type(type) {}

	Entity entity;
	Entity owner;
	uint8_t x;
	uint8_t y;
	BombType type;
};
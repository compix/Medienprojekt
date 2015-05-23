#pragma once
#include <entityx/Entity.h>

struct BombExplodedEvent
{
	BombExplodedEvent(entityx::Entity bomb)
		: bomb(bomb) {}

	entityx::Entity bomb;
};
#pragma once
#include <entityx/Entity.h>

struct GameOverEvent
{
	GameOverEvent(entityx::Entity lastPlayer)
		: lastPlayer(lastPlayer) {}

	entityx::Entity lastPlayer;
};

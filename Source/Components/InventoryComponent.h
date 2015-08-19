#pragma once
#include "../GameConstants.h"
#include <utility>
#include <entityx/Entity.h>


struct InventoryComponent
{
	InventoryComponent() : bombCount(GameConstants::INIT_BOMB_COUNT), explosionRange(GameConstants::INIT_BOMB_RANGE), speedMultiplicator(GameConstants::INIT_SPEED_MULTI), bombKick(GameConstants::INIT_PLAYERS_CAN_KICK), portalSkill(GameConstants::INIT_PORTAL_SKILL){}

	int bombCount;
	int explosionRange;
	float speedMultiplicator;
	bool bombKick;
	bool portalSkill;
	std::pair<entityx::Entity, entityx::Entity> placedPortals;
};
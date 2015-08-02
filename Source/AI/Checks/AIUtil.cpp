#include "AIUtil.h"

float AIUtil::getTimePerCell(entityx::Entity& entity)
{
	assert(entity.has_component<InventoryComponent>());
	auto inventory = entity.component<InventoryComponent>();

	float speed = inventory->speedMultiplicator * GameConstants::PLAYER_SPEED;

	return GameConstants::CELL_WIDTH / GameConstants::S_SCALE / speed;
}
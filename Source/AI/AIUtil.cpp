#include "AIUtil.h"
#include "../Components/InventoryComponent.h"
#include "../GameConstants.h"
#include "PathFinding/AIPath.h"

float AIUtil::getTimePerCell(entityx::Entity& entity)
{
	assert(entity.has_component<InventoryComponent>());
	auto inventory = entity.component<InventoryComponent>();

	float speed = inventory->speedMultiplicator * GameConstants::PLAYER_SPEED;

	return GameConstants::CELL_WIDTH / GameConstants::S_SCALE / speed;
}

bool AIUtil::isSafePath(entityx::Entity& entity, AIPath& path, float* minExplosionTime)
{
	// How long does a player approx. need to get from one cell to another if the path is NOT blocked (worst case)?
	float timePerCell = getTimePerCell(entity);

	if (minExplosionTime)
		*minExplosionTime = 2.f;

	for (int i = 0; i < path.nodes.size(); ++i)
	{
		auto node = path.nodes[i];
		if (node->properties.affectedByExplosion)
		{
			float exploTime = node->properties.timeTillExplosion - timePerCell * i;
			if (minExplosionTime)
				*minExplosionTime = std::min(*minExplosionTime, exploTime);
			if (exploTime >= -(timePerCell + GameConstants::AI_UPDATE_TIME * 2.f) && exploTime <= timePerCell + GameConstants::AI_UPDATE_TIME * 2.f)
				return false;
		}
	}

	return true;
}
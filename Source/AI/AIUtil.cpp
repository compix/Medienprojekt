#include "AIUtil.h"
#include "../Components/InventoryComponent.h"
#include "../GameConstants.h"
#include "PathFinding/AIPath.h"
#include "../PhysixSystem.h"
#include "../Components/CellComponent.h"

float AIUtil::getTimePerCell(entityx::Entity& entity)
{
	// How long does a player approx. need to get from one cell to another (worst case)?
	assert(entity.has_component<InventoryComponent>());
	auto inventory = entity.component<InventoryComponent>();

	// Measured in pixels/s
	// Multiplication with m_Scale is the transformation from meters to pixels
	float speed = inventory->speedMultiplicator * GameConstants::PLAYER_SPEED * PhysixSystem::m_Scale;

	return GameConstants::CELL_WIDTH / speed;
}

bool AIUtil::isSafePath(entityx::Entity& entity, AIPath& path, float* minExplosionTime)
{
	float timePerCell = getTimePerCell(entity);

	if (minExplosionTime)
		*minExplosionTime = 2.f;

	for (int i = path.curNode; i < path.nodes.size(); ++i)
	{
		auto node = path.nodes[i];
		if (node->properties.affectedByExplosion)
		{
			float exploTime = node->properties.timeTillExplosion - timePerCell * (i - path.curNode);
			if (minExplosionTime)
				*minExplosionTime = std::min(*minExplosionTime, exploTime);
			if (exploTime <= timePerCell + GameConstants::AI_UPDATE_TIME * 2.f)
				return false;
		}
	}

	return true;
}

bool AIUtil::isOnPath(entityx::Entity& entity, const AIPath& path)
{
	assert(entity.has_component<CellComponent>());
	auto cell = entity.component<CellComponent>();
	for (auto node : path.nodes)
		if (node->x == cell->x && node->y == cell->y)
			return true;

	return false;
}

bool AIUtil::isBlockedExceptLast(const AIPath& path)
{
	for (int i = path.curNode; i < path.nodes.size() - 1; ++i)
	{
		if (!path.nodes[i]->valid)
			return true;
	}

	return false;
}
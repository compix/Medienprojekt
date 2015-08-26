#include "AIUtil.h"
#include "../Components/InventoryComponent.h"
#include "../GameConstants.h"
#include "PathFinding/AIPath.h"
#include "../PhysixSystem.h"
#include "../Components/CellComponent.h"
#include "../Components/TransformComponent.h"

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
	assert(entity.has_component<InventoryComponent>() && entity.has_component<TransformComponent>());

	if (minExplosionTime)
		*minExplosionTime = 2.f;

	float timePerCell = getTimePerCell(entity);

	// Exact time from the current node to the next on the path
	float timeToNext = timePerCell;
	if (uint16_t(path.curNode + 1) < path.nodes.size())
	{
		auto inventory = entity.component<InventoryComponent>();
		float speed = inventory->speedMultiplicator * GameConstants::PLAYER_SPEED * PhysixSystem::m_Scale;
		auto transform = entity.component<TransformComponent>();

		auto n1 = path.nodes[path.curNode];
		auto n2 = path.nodes[path.curNode + 1];
		n2 = n2->properties.otherPortal ? n2->properties.otherPortal : n2;

		int dx = n2->x - n1->x;
		int dy = n2->y - n1->y;

		int cellX = int(transform->x / GameConstants::CELL_WIDTH);
		int cellY = int(transform->y / GameConstants::CELL_HEIGHT);

		// If the entity is on that cell
		if (n1->x == cellX && n1->y == cellY && (abs(dx) + abs(dy) == 1))
		{
			float xBorder = float(n2->x * GameConstants::CELL_WIDTH + (dx == 1 ? 0 : GameConstants::CELL_WIDTH));
			float yBorder = float(n2->y * GameConstants::CELL_HEIGHT + (dy == 1 ? 0 : GameConstants::CELL_HEIGHT));

			float distance = (abs(dx) == 1) ? abs(transform->x - xBorder) : abs(transform->y - yBorder);
			timeToNext = distance / speed;
		}
	}

	float travelTime = 0.f;
	for (uint32_t i = path.curNode; i < path.nodes.size(); ++i)
	{
		auto node = path.nodes[i];
		if (node->properties.affectedByExplosion)
		{
			float exploTime = node->properties.timeTillExplosion - travelTime;
			if (minExplosionTime)
				*minExplosionTime = std::min(*minExplosionTime, exploTime);
			if (exploTime <= timePerCell * 1.5f + GameConstants::AI_UPDATE_TIME * 3.f)
				return false;
		}

		travelTime = timePerCell * (i - path.curNode) + timeToNext;
	}

	return true;
}

bool AIUtil::isOnPath(entityx::Entity& entity, const AIPath& path)
{
	assert(entity.has_component<CellComponent>());
	auto cell = entity.component<CellComponent>();
	for (auto node : path.nodes)
	{
		if (node->x == cell->x && node->y == cell->y)
			return true;

		auto portal = node->properties.otherPortal;
		if (portal && portal->x == cell->x && portal->y == cell->y)
			return true;
	}

	return false;
}

bool AIUtil::isBlockedIgnoreLast(const AIPath& path)
{
	for (uint32_t i = path.curNode + 1; i < path.nodes.size() - 1; ++i)
	{
		if (!path.nodes[i]->valid)
			return true;
	}

	return false;
}

bool AIUtil::isValidPath(const AIPath& path)
{
	for (uint32_t i = path.curNode; i < path.nodes.size() - 1; ++i)
	{
		auto n1 = path.nodes[i];
		auto n2 = path.nodes[i + 1];
		int distance = abs(n1->x - n2->x) + abs(n1->y - n2->y);

		if (distance > 1 && !n2->properties.otherPortal)
			return false; // portal disappeared, there is a jump in cell distance now
	}

	return true;
}
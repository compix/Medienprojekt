#include "RateEscape.h"
#include "../../Components/CellComponent.h"
#include "../AIUtil.h"
#include "../PathFinding/PathEngine.h"
#include "../../Systems/AISystem.h"

bool RateEscape::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	if (!path.goal()->valid)
		return false;

	AISystem::getEnemies(entity, m_entitiesToAvoid);

	auto goal = path.goal();
	int newDistanceToEnemies = computeDistanceToEnemies(goal->x, goal->y);

	if (m_entitiesToAvoid.size() == 0)
	{
		path.rating = 2.f;
		return true;
	}

	if (AIUtil::isSafePath(entity, path))
	{
		path.rating = distanceToClosest(goal->x, goal->y) > 4 ? 2.f : 0.f;
		return true;
	}

	return false;
}

int RateEscape::computeDistanceToEnemies(uint8_t x, uint8_t y)
{
	int distance = 0;

	for (auto& e : m_entitiesToAvoid)
	{
		assert(e.valid() && e.has_component<CellComponent>());
		auto cell = e.component<CellComponent>();

		distance += abs(cell->x - x) + abs(cell->y - y);
	}

	return distance;
}

int RateEscape::computeAccumulatedDistance(AIPath& path)
{
	int sum = 0;

	for (uint32_t i = 0; i < path.nodes.size(); ++i)
	{
		auto node = path.nodes[i];
		int distance = computeDistanceToEnemies(node->x, node->y);
		sum += (distance) / path.nodes.size();
	}
	
	return sum;
}

int RateEscape::distanceToClosest(uint8_t x, uint8_t y)
{
	int distance = 10;

	for (auto& e : m_entitiesToAvoid)
	{
		assert(e.valid() && e.has_component<CellComponent>());
		auto cell = e.component<CellComponent>();

		distance = std::min(distance, abs(cell->x - x) + abs(cell->y - y));
	}

	return distance;
}
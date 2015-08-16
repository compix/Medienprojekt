#include "RateEscape.h"
#include "../../Components/CellComponent.h"
#include "../AIUtil.h"
#include "../PathFinding/PathEngine.h"

RateEscape::RateEscape(entityx::Entity& entity, std::vector<entityx::Entity>& entitiesToAvoid)
	:m_entitiesToAvoid(entitiesToAvoid), m_entity(entity)
{
	assert(m_entity.valid() && m_entity.has_component<CellComponent>());
	auto cell = m_entity.component<CellComponent>();
	m_startDistance = computeDistanceToEnemies(cell->x, cell->y);
}

bool RateEscape::operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum)
{
	int newDistanceToEnemies = computeDistanceToEnemies(node->x, node->y);

	if (m_entitiesToAvoid.size() == 0)
	{
		pathOut.rating = 2.f;
		return true;
	}

	pathEngine->makePath(pathOut, node, taskNum);

	if (AIUtil::isSafePath(m_entity, pathOut))
	{
		pathOut.rating = distanceToClosest(node->x, node->y) > 4 ? 2.f : 0.f;
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

	for (int i = 0; i < path.nodes.size(); ++i)
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
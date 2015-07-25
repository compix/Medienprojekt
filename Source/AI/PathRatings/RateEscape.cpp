#include "RateEscape.h"
#include "../Checks/IsSafePath.h"
#include "../../Components/CellComponent.h"

RateEscape::RateEscape(entityx::Entity self, std::vector<entityx::Entity>& entitiesToAvoid)
	:m_entitiesToAvoid(entitiesToAvoid), m_self(self)
{
	assert(m_self.valid() && m_self.has_component<CellComponent>());
	auto cell = m_self.component<CellComponent>();
	m_startDistance = computeDistanceToEnemies(cell->x, cell->y);
}

bool RateEscape::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	IsSafePath isSafePath;

	int newDistanceToEnemies = computeDistanceToEnemies(node->x, node->y);

	if (m_entitiesToAvoid.size() == 0)
	{
		pathOut.rating = 2.f;
		return true;
	}

	pathEngine->makePath(pathOut, node, taskNum);

	if (isSafePath(pathOut))
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

int RateEscape::computeAccumulatedDistance(Path& path)
{
	int sum = 0;

	for (int i = 0; i < path.nodeCount; ++i)
	{
		auto node = path.nodes[i];
		int distance = computeDistanceToEnemies(node->x, node->y);
		sum += (distance) / path.nodeCount;
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
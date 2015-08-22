#include "RateTrapDanger.h"
#include "../PathFinding/PathEngine.h"
#include "../../Components/CellComponent.h"
#include "../../Systems/AISystem.h"

RateTrapDanger::RateTrapDanger(bool willPlaceBomb)
	:m_willPlaceBomb(willPlaceBomb)
{
}

bool RateTrapDanger::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();

	if (!goal->valid)
		return false;

	auto graph = pathEngine->getSimGraph();
	uint8_t range = 2;

	bool trap = isPotentialTrap(pathEngine, goal) ||
				isPotentialTrap(graph, goal, Direction::DOWN, range) ||
				isPotentialTrap(graph, goal, Direction::UP, range)   ||
				isPotentialTrap(graph, goal, Direction::LEFT, range) ||
				isPotentialTrap(graph, goal, Direction::RIGHT, range);

	AISystem::getEnemies(entity, m_enemies);

	if (trap)
		path.rating = 0.f;
	else
		path.rating = 2.f;

	return true;
}

bool RateTrapDanger::isPotentialTrap(PathEngine* pathEngine, GraphNode* node)
{
	if (distanceToClosest(node->x, node->y) > 5)
		return false;

	auto graph = pathEngine->getSimGraph();

	bool upBlocked    = !graph->hasNeighbor(node, Direction::UP);
	bool downBlocked  = !graph->hasNeighbor(node, Direction::DOWN);
	bool leftBlocked  = !graph->hasNeighbor(node, Direction::LEFT);
	bool rightBlocked = !graph->hasNeighbor(node, Direction::RIGHT);

	return (upBlocked && downBlocked && (leftBlocked || rightBlocked)) || (leftBlocked && rightBlocked && (upBlocked || downBlocked));
}


bool RateTrapDanger::isPotentialTrap(SimulationGraph* graph, GraphNode* node, Direction direction, uint8_t range)
{
	if (direction == Direction::LEFT || direction == Direction::RIGHT)
	{
		auto curNode = node;
		for (uint8_t i = 0; curNode && i < range; ++i)
		{
			curNode = graph->getNeighbor(curNode, direction);
			if (distanceToClosest(curNode->x, curNode->y) > 5)
				return false;

			if (downUpBlocked(graph, curNode) && !graph->hasNeighbor(curNode, direction))
				return true;
		}
	}
	else
	{
		auto curNode = node;
		for (uint8_t i = 0; curNode && i < range; ++i)
		{
			curNode = graph->getNeighbor(curNode, direction);
			if (distanceToClosest(curNode->x, curNode->y) > 5)
				return false;

			if (leftRightBlocked(graph, curNode) && !graph->hasNeighbor(curNode, direction))
				return true;
		}
	}

	return false;
}

bool RateTrapDanger::downUpBlocked(SimulationGraph* graph, GraphNode* node)
{
	return !graph->hasNeighbor(node, Direction::UP) && !graph->hasNeighbor(node, Direction::DOWN);
}

bool RateTrapDanger::leftRightBlocked(SimulationGraph* graph, GraphNode* node)
{
	return !graph->hasNeighbor(node, Direction::LEFT) && !graph->hasNeighbor(node, Direction::RIGHT);
}

int RateTrapDanger::distanceToClosest(uint8_t x, uint8_t y)
{
	int distance = 500;

	for (auto& e : m_enemies)
	{
		assert(e.valid() && e.has_component<CellComponent>());
		auto cell = e.component<CellComponent>();

		int newDistance = abs(cell->x - x) + abs(cell->y - y);
		if (newDistance < distance)
		{
			distance = newDistance;
		}
	}

	return distance;
}


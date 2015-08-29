#include "RateTrapDanger.h"
#include "../PathFinding/PathEngine.h"
#include "../../Components/CellComponent.h"
#include "../../Systems/AISystem.h"
#include "../../Components/AIComponent.h"

RateTrapDanger::RateTrapDanger()
{
}

bool RateTrapDanger::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();

	if (!goal->valid)
		return false;

	std::vector<entityx::Entity> enemies;
	AIUtil::getEnemies(entity, enemies);

	auto graph = pathEngine->getSimGraph();
	uint8_t range = 2;

	bool trap = false;

	// Check only for a trap if an enemy is nearby
	if (goal->smell(SmellType::ENEMY) > 0)
	{
		// Check current node
		trap = isPotentialTrap(pathEngine, goal, range);

		// Check in all directions if it's a potential trap
		for (uint8_t i = 0; !trap && i < 4; ++i)
		{
			Direction direction = static_cast<Direction>(i);
			trap = isPotentialTrap(graph, goal, direction, range);
		}
	}

	auto& personality = entity.component<AIComponent>()->personality;
	auto& desires = personality.desires;
	auto& affinity = personality.affinity;
	path.rating = (affinity.getSafe * desires.getSafe) * (trap ? -1.f : 1.f);

	return true;
}

bool RateTrapDanger::isPotentialTrap(PathEngine* pathEngine, GraphNode* node, uint8_t range)
{
	auto graph = pathEngine->getSimGraph();

	bool upBlocked    = !graph->hasNeighbor(node, Direction::UP);
	bool downBlocked  = !graph->hasNeighbor(node, Direction::DOWN);
	bool leftBlocked  = !graph->hasNeighbor(node, Direction::LEFT);
	bool rightBlocked = !graph->hasNeighbor(node, Direction::RIGHT);

	return (upBlocked && downBlocked && (leftBlocked || rightBlocked)) || (leftBlocked && rightBlocked && (upBlocked || downBlocked));
}


bool RateTrapDanger::isPotentialTrap(SimulationGraph* graph, GraphNode* node, Direction direction, uint8_t range)
{
	assert(node);
	if (direction == Direction::LEFT || direction == Direction::RIGHT)
	{
		auto curNode = node;
		for (uint8_t i = 0; graph->hasNeighbor(curNode, direction) && i < range; ++i)
		{
			curNode = graph->getNeighbor(curNode, direction);

			if (downUpBlocked(graph, curNode) && !graph->hasNeighbor(curNode, direction))
				return true;
		}
	}
	else
	{
		auto curNode = node;
		for (uint8_t i = 0; graph->hasNeighbor(curNode, direction) && i < range; ++i)
		{
			curNode = graph->getNeighbor(curNode, direction);

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
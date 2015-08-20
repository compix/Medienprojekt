#include "RateTrapDanger.h"
#include "../PathFinding/PathEngine.h"
#include "../../Components/CellComponent.h"
#include "RateSafety.h"
#include "../PathFinding/SimulationGraph.h"
#include "../../Systems/AISystem.h"

RateTrapDanger::RateTrapDanger(bool willPlaceBomb)
	:m_willPlaceBomb(willPlaceBomb)
{
}

bool RateTrapDanger::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	auto goal = path.goal();

	if (!goal->valid)
		return false;

	auto simGraph = pathEngine->getSimGraph();
	simGraph->resetMarks();
	uint8_t maxIterations = 10;

	AISystem::getEnemies(entity, m_enemies);

	// Test neighbors in a breadth first search manner
	std::queue<GraphNode*> processQueue;
	auto startNode = goal;
	startNode->marked = true;
	processQueue.push(startNode);

	while (maxIterations > 0 && processQueue.size() > 0) 
	{
		GraphNode* curNode = processQueue.front();
		processQueue.pop();

		if (!testNode(startNode, curNode, entity, pathEngine, taskNum))
			return false;

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			GraphNode* neighbor = simGraph->getNeighbor(curNode, static_cast<Direction>(i));
			if (!neighbor->valid || neighbor->marked)
				continue;

			neighbor->marked = true;
			processQueue.push(neighbor);
		}

		--maxIterations;
	}

	path.rating = 1.f;
	return true;
}


int RateTrapDanger::distanceToClosest(uint8_t x, uint8_t y, entityx::Entity& closestEnemy)
{
	int distance = 500;

	for (auto& e : m_enemies)
	{
		assert(e.valid() && e.has_component<CellComponent>());
		auto cell = e.component<CellComponent>();

		float newDistance = abs(cell->x - x) + abs(cell->y - y);
		if (newDistance < distance)
		{
			distance = newDistance;
			closestEnemy = e;
		}
	}

	return distance;
}

bool RateTrapDanger::testNode(GraphNode* startNode, GraphNode* testedNode, entityx::Entity& entity, PathEngine* pathEngine, uint8_t taskNum)
{
	Entity closestEnemy;
	int distanceToClosestEnemy = distanceToClosest(testedNode->x, testedNode->y, closestEnemy);

	if (distanceToClosestEnemy > 5)
		return true;

	// Distance check not enough. Check the path
	AIPath enemyPathToSpot;
	auto enemyCell = closestEnemy.component<CellComponent>();
	pathEngine->computePath(enemyCell->x, enemyCell->y, startNode->x, startNode->y, enemyPathToSpot, taskNum + 1);

	if (enemyPathToSpot.nodes.size() > 5 || enemyPathToSpot.nodes.size() == 0)
		return true;

	auto simGraph = pathEngine->getSimGraph();

	float bombExploTime = testedNode->properties.affectedByExplosion ? testedNode->properties.timeTillExplosion : 2.f;
	simGraph->placeBomb(testedNode->x, testedNode->y, 7, 2.f);
	testedNode->valid = false;

	if (m_willPlaceBomb)
	{
		simGraph->placeBomb(startNode->x, startNode->y, 7, 2.f);
		startNode->valid = false;
	}

	AIPath safePath;
	pathEngine->breadthFirstSearch(entity, startNode->x, startNode->y, safePath, RateSafety(), taskNum + 1);
	bool safe = safePath.nodes.size() > 0;

	// Reset to old state
	simGraph->resetSimulation();
	testedNode->valid = true;
	startNode->valid = true;

	return safe;
}
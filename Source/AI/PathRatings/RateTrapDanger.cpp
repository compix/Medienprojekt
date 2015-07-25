#include "RateTrapDanger.h"
#include "../../Utils/PathFinding/PathEngine.h"
#include "../../Components/CellComponent.h"
#include "../Checks/IsSafePath.h"
#include "RateSafety.h"
#include "../../Utils/PathFinding/SimulationGraph.h"

RateTrapDanger::RateTrapDanger(entityx::Entity self, std::vector<entityx::Entity>& enemies, bool willPlaceBomb)
	:m_self(self), m_enemies(enemies), m_willPlaceBomb(willPlaceBomb)
{
}

bool RateTrapDanger::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	auto simGraph = pathEngine->getSimGraph();
	simGraph->resetMarks();
	uint8_t maxIterations = 10;

	// Test neighbors in a breadth first search manner
	std::queue<GraphNode*> processQueue;
	auto startNode = node;
	startNode->marked = true;
	processQueue.push(startNode);

	while (maxIterations > 0 && processQueue.size() > 0) 
	{
		GraphNode* curNode = processQueue.front();
		processQueue.pop();

		if (!testNode(startNode, curNode, pathEngine, taskNum))
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

	pathOut.rating = 1.f;
	return true;
}


int RateTrapDanger::distanceToClosest(uint8_t x, uint8_t y)
{
	int distance = 500;

	for (auto& e : m_enemies)
	{
		assert(e.valid() && e.has_component<CellComponent>());
		auto cell = e.component<CellComponent>();

		distance = std::min(distance, abs(cell->x - x) + abs(cell->y - y));
	}

	return distance;
}

bool RateTrapDanger::testNode(GraphNode* startNode, GraphNode* testedNode, PathEngine* pathEngine, uint8_t taskNum)
{

	int distanceToClosestEnemy = distanceToClosest(testedNode->x, testedNode->y);

	if (distanceToClosestEnemy > 5)
		return true;

	IsSafePath isSafePath;
	auto simGraph = pathEngine->getSimGraph();

	float bombExploTime = testedNode->properties.affectedByExplosion ? testedNode->properties.timeTillExplosion : 2.f;
	simGraph->placeBomb(testedNode->x, testedNode->y, 7, 2.f);
	testedNode->valid = false;

	if (m_willPlaceBomb)
	{
		simGraph->placeBomb(startNode->x, startNode->y, 7, 2.f);
		startNode->valid = false;
	}

	Path safePath;
	pathEngine->breadthFirstSearch(startNode->x, startNode->y, safePath, RateSafety(), taskNum + 1);
	bool safe = safePath.nodeCount > 0;

	// Reset to old state
	simGraph->resetSimulation();
	testedNode->valid = true;
	startNode->valid = true;

	return safe;
}
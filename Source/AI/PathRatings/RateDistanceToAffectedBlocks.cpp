#include "RateDistanceToAffectedBlocks.h"
#include "../../Components/CellComponent.h"
#include "../../Components/ItemComponent.h"
#include "../../Game.h"

RateDistanceToAffectedBlocks::RateDistanceToAffectedBlocks(entityx::Entity& entity)
{
}

bool RateDistanceToAffectedBlocks::operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum)
{
	if (pathEngine->getGraph()->getAffectedBlocks().size() == 0)
		return false;

	float valueFactor = 15.f;
	pathOut.rating = inverseDistanceToAffectedBlocks(pathEngine->getGraph(), node) * valueFactor;
	return true;
}

float RateDistanceToAffectedBlocks::inverseDistanceToAffectedBlocks(Graph* graph, GraphNode* node)
{
	assert(graph->getAffectedBlocks().size() > 0);

	float invDistance = 0;
	for (auto& block : graph->getAffectedBlocks())
	{
		if (!block.valid())
			continue;

		auto blockCell = block.component<CellComponent>();
		float distance = abs(node->x - blockCell->x) + abs(node->y - blockCell->y);
		assert(distance > 0.f); // The entity can never be on the same cell as the block

		// Use the squared distance for a higher evaluation of close blocks and lower evaluation for blocks that are far away
		invDistance += 1.f / (distance * distance);
	}

	return invDistance / graph->getAffectedBlocks().size();
}
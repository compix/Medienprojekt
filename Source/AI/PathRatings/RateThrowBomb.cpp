#include "RateThrowBomb.h"
#include "RateAttackEnemy.h"

bool RateThrowBomb::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();
	if (!goal->valid)
		return false;

	auto inventory = entity.component<InventoryComponent>();
	if (inventory->getAvailableBombCount() == 0)
		return false;

	auto graph = pathEngine->getSimGraph();

	// Check in all directions if the bomb can be thrown
	for (uint8_t i = 0; i < 4; ++i)
	{
		Direction direction = static_cast<Direction>(i);
		auto expectedNode = getExpectedBombLanding(pathEngine, goal, direction);
		if (expectedNode)
		{
			RateAttackEnemy rateAttackEnemy;
			AIPath attackPath = path;
			if (rateAttackEnemy(pathEngine, attackPath, entity))
			{
				path.rating = attackPath.rating;
				path.behaviorNode = graph->getNeighborIgnorePortal(goal, direction);
				return true;
			}
		}
	}

	return false;
}

GraphNode* RateThrowBomb::getExpectedBombLanding(PathEngine* pathEngine, GraphNode* startNode, Direction direction)
{
	int range = GameConstants::PUNCH_DISTANCE;
	auto node = startNode;
	auto graph = pathEngine->getSimGraph();

	while (node && (!node->valid || range > 0))
	{
		--range;

		// Bomb flies over poratls so ignore portals
		if (range > 0)
			node = graph->getNeighborIgnorePortal(node, direction);
		else
			node = graph->getNeighbor(node, direction);
	}

	return node;
}
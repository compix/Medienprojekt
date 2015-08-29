#include "RateThrowBomb.h"
#include "RateAttackEnemy.h"

bool RateThrowBomb::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();
	if (!goal->valid)
		return false;

	auto inventory = entity.component<InventoryComponent>();
	if (inventory->getAvailableBombCount() == 0 || !inventory->canHoldBomb())
		return false;

	if (!AIUtil::isSafePath(entity, path))
		return false;

	if (goal->smell(SmellType::ENEMY) == 0)
		return false;

	auto graph = pathEngine->getSimGraph();

	// Check in all directions if the bomb can be thrown
	for (uint8_t i = 0; i < 4; ++i)
	{
		Direction direction = static_cast<Direction>(i);
		uint8_t throwDistance;
		auto expectedNode = getExpectedBombLanding(pathEngine, goal, direction, throwDistance);
		// Don't throw the bomb too far
		if (expectedNode && throwDistance <= 4)
		{
			RateAttackEnemy rateAttackEnemy;
			AIPath attackPath;
			attackPath.nodes.push_back(expectedNode);
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

GraphNode* RateThrowBomb::getExpectedBombLanding(PathEngine* pathEngine, GraphNode* startNode, Direction direction, uint8_t& outThrowDistance)
{
	int range = GameConstants::PUNCH_DISTANCE;
	auto node = startNode;
	auto graph = pathEngine->getSimGraph();
	outThrowDistance = 0;

	while (node && (!node->valid || range > 0))
	{
		--range;
		++outThrowDistance;

		// Bomb flies over poratls so ignore portals
		if (range > 0)
			node = graph->getNeighborIgnorePortal(node, direction);
		else
			node = graph->getNeighbor(node, direction);
	}

	return node;
}
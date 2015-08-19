#include "RateKickBomb.h"

bool RateKickBomb::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	auto goal = path.goal();
	auto graph = pathEngine->getGraph();	

	if (!goal->properties.hasBomb || path.nodes.size() < 2)
		return false;

	// Check if the the bomb can be kicked
	auto prevNode = path.nodes[path.nodes.size() - 2];
	int dx = static_cast<int>(goal->x) - static_cast<int>(prevNode->x);
	int dy = static_cast<int>(goal->y) - static_cast<int>(prevNode->y);

	Direction kickDirection = CommonUtil::toDirection(dx, dy);

	auto neighbor = graph->getNeighbor(goal, kickDirection);

	if (neighbor->valid && !neighbor->properties.hasPlayer)
	{
		// Kicking a bomb with a player on it is risky -> he could move to make kicking the bomb impossible
		path.rating = goal->properties.hasPlayer ? 0.5f : 1.f;
		return true;
	}

	return false;
}
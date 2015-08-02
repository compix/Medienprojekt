#include "RateKickBomb.h"
#include "../Checks/AIUtil.h"

RateKickBomb::RateKickBomb(entityx::Entity& entity)
	:m_entity(entity)
{
}

bool RateKickBomb::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	auto graph = pathEngine->getGraph();	

	// Check if the neighbor has a bomb and if the bomb can be kicked in that direction
	for (int i = 0; i < 4; ++i)
	{
		Direction dir = static_cast<Direction>(i);
		auto neighbor = graph->getNeighbor(node, dir);

		if (neighbor->properties.hasBomb)
		{
			int dx = static_cast<int>(neighbor->x) - static_cast<int>(node->x);
			int dy = static_cast<int>(neighbor->y) - static_cast<int>(node->y);

			Direction kickDirection = CommonUtil::toDirection(dx, dy);

			auto neighborOfNeighbor = graph->getNeighbor(neighbor, kickDirection);

			if (neighborOfNeighbor->valid && !neighborOfNeighbor->properties.hasPlayer)
			{
				pathEngine->makePath(pathOut, node, taskNum);
				pathOut.nodes.push_back(neighbor);
				pathOut.nodeCount++;
				pathOut.rating = neighbor->properties.hasPlayer ? 0.5f : 1.f;
				return true;
			}
		}
	}

	return false;
}
#include "../../Components/InventoryComponent.h"
#include "../../Components/CellComponent.h"
#include "RatePortalSpot.h"

RatePortalSpot::RatePortalSpot(entityx::Entity& entity)
	:m_entity(entity)
{
}

bool RatePortalSpot::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	auto inventory = m_entity.component<InventoryComponent>();

	auto firstPortal = inventory->placedPortals.first;
	if (!firstPortal.valid())
	{
		// Doesn't really matter where to place it (no idea what spots are strategically better)
		pathEngine->makePath(pathOut, node, taskNum);
		pathOut.rating = 1.f;
		return true;
	}

	auto cell = firstPortal.component<CellComponent>();
	if (pathEngine->getGraph()->getNode(cell->x, cell->y)->properties.affectedByExplosion)
		return false;

	auto secondPortal = inventory->placedPortals.second;
	if (!secondPortal.valid())
	{
		// Higher distance between portals = good spot. I guess.
			
		float distance = abs(cell->x - node->x) + abs(cell->y - node->y);
		if (distance < 5)
			return false;

		pathEngine->makePath(pathOut, node, taskNum);
		pathOut.rating = distance*distance / pathOut.nodeCount;
		return true;
	}

	return false;
}
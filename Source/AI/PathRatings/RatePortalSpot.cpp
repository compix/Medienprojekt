#include "../../Components/InventoryComponent.h"
#include "../../Components/CellComponent.h"
#include "RatePortalSpot.h"

bool RatePortalSpot::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	auto goal = path.goal();

	if (!goal->valid)
		return false;

	// Check if the player can place portals
	assert(entity.has_component<InventoryComponent>());
	auto inventory = entity.component<InventoryComponent>();
	if (!inventory->portalSkill)
		return false;

	// Can't place a portal on a portal
	if (goal->properties.hasPortal)
		return false;

	auto firstPortal = inventory->placedPortals.first;
	if (!firstPortal.valid())
	{
		// Doesn't really matter where to place it (no idea what spots are strategically better)
		path.rating = 1.f;
		return true;
	}

	auto cell = firstPortal.component<CellComponent>();

	// Don't place a portal if the other portal is affected by an explosion
	if (pathEngine->getGraph()->getNode(cell->x, cell->y)->properties.affectedByExplosion)
		return false;

	auto secondPortal = inventory->placedPortals.second;
	if (!secondPortal.valid())
	{
		// Higher distance between portals = good spot. I guess.	
		int distance = abs(cell->x - goal->x) + abs(cell->y - goal->y);
		if (distance < 5)
			return false;

		path.rating = float(distance*distance) / path.nodes.size();
		return true;
	}

	return false;
}
#include "../../Components/InventoryComponent.h"
#include "../../Components/CellComponent.h"
#include "RatePortalSpot.h"
#include "../../Components/AIComponent.h"

bool RatePortalSpot::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();

	if (!goal->valid || goal->properties.affectedByExplosion)
		return false;

	// Check if the player can place portals
	assert(entity.has_component<InventoryComponent>());
	auto inventory = entity.component<InventoryComponent>();
	if (!inventory->isActive(SkillType::PLACE_PORTAL))
		return false;

	// Can't place a portal on a portal
	if (goal->properties.hasPortal)
		return false;

	auto& personality = entity.component<AIComponent>()->personality;
	auto& desires = personality.desires;
	auto& affinity = personality.affinity;

	auto firstPortal = inventory->placedPortals.first;
	if (!firstPortal.valid())
	{
		// Doesn't really matter where to place it (no idea what spots are strategically better)
		path.rating = affinity.placePortal * desires.placePortal;
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

		path.rating = (affinity.placePortal + float(distance)) * desires.placePortal;
		return true;
	}

	return false;
}
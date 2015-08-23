#include "RateAttackEnemy.h"
#include "../AIUtil.h"
#include "RateSafety.h"
#include "../../Components/InventoryComponent.h"

bool RateAttackEnemy::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();

	if (goal->valid && !goal->properties.hasPortal) // !node->properties.hasPortal TEMPORARY BECAUSE PORTALS ARE BUGGED
	{
		// If the AI can't place more bombs then it fails
		assert(entity.has_component<InventoryComponent>());
		auto inventory = entity.component<InventoryComponent>();
		if (inventory->bombCount == 0)
			return false;

		if (AIUtil::isSafePath(entity, path))
		{
			// Bomb should affect enemies but no items
			bool found = true;
			float timePerCell = AIUtil::getTimePerCell(entity);
			float bombExploTime = goal->properties.affectedByExplosion ? goal->properties.timeTillExplosion : GameConstants::EXPLOSION_TIME;
			bool spotAffectedByExplosion = goal->properties.affectedByExplosion;

			AffectedByExplosion affectedEntities;
			pathEngine->getSimGraph()->placeBomb(goal->x, goal->y, inventory->explosionRange, bombExploTime, &affectedEntities);

			bool enemiesAffected = affectedEntities.isEnemyAffected(entity);
			bool itemsAffected = affectedEntities.numOfItems > 0;

			if (!enemiesAffected || itemsAffected)
				found = false;

			if (found)
			{
				// Found a potential spot -> check if a safe escape path exists
				AIPath safePath;
				pathEngine->searchBest(entity, goal->x, goal->y, safePath, RateSafety(), 1);

				if (safePath.nodes.size() < 2 || !AIUtil::isSafePath(entity, safePath))
					found = false;

				if (found && spotAffectedByExplosion)
				{
					// Check the full path to be sure if it's really safe
					AIPath fullPath;
					fullPath.attach(path);
					fullPath.attach(safePath);
					fullPath.curNode = path.curNode;

					if (!AIUtil::isSafePath(entity, fullPath))
						found = false;
				}
			}

			if (found)
				path.rating = 5.f - timePerCell * path.nodes.size();

			// Reset to the old state
			pathEngine->getSimGraph()->resetSimulation();

			return found;
		}
	}

	return false;
}
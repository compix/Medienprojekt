#include "RateDestroyBlockSpot.h"
#include "../../GameConstants.h"
#include "../../Components/BlockComponent.h"
#include "RateSafety.h"
#include "../PathFinding/PathEngine.h"
#include "../AIUtil.h"

bool RateDestroyBlockSpot::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	auto goal = path.goal();

	if (goal->valid && !goal->properties.hasPortal) // !node->properties.hasPortal TEMPORARY BECAUSE PORTALS ARE BUGGED
	{
		if (AIUtil::isSafePath(entity, path))
		{
			// Bomb should affect blocks but no items
			bool found = true;
			float timePerCell = AIUtil::getTimePerCell(entity);
			float bombExploTime = goal->properties.affectedByExplosion ? goal->properties.timeTillExplosion : 2.f; // TODO: replace 2.f
			bool spotAffectedByExplosion = goal->properties.affectedByExplosion;		

			pathEngine->getSimGraph()->placeBomb(goal->x, goal->y, 7, bombExploTime); // TODO replace the range

			bool blocksAffected = goal->properties.numOfBlocksAffectedByExplosion > 0;
			bool itemsAffected = goal->properties.numOfItemsAffectedByExplosion > 0;

			if (!blocksAffected || itemsAffected)
				found = false;

			if (found)
			{
				// Found a potential spot -> check if a safe escape path exists
				AIPath safePath;
				pathEngine->breadthFirstSearch(entity, goal->x, goal->y, safePath, RateSafety(), taskNum + 1);

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
				path.rating = goal->properties.numOfBlocksAffectedByExplosion - timePerCell * path.nodes.size();

			// Reset to the old state
			pathEngine->getSimGraph()->resetSimulation();

			return found;
		}
	}

	return false;
}
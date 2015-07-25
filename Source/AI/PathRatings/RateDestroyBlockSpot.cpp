#include "RateDestroyBlockSpot.h"
#include "../../GameConstants.h"
#include "../Checks/IsSafePath.h"
#include "../../Components/BlockComponent.h"
#include "RateSafety.h"
#include "../../Utils/PathFinding/SimulationGraph.h"

bool RateDestroyBlockSpot::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	IsSafePath isSafePath;

	if (node->valid)
	{
		pathEngine->makePath(pathOut, node, taskNum);
		if (isSafePath(pathOut))
		{
			bool found = true;
			float timePerCell = (GameConstants::CELL_WIDTH / GameConstants::S_SCALE) / GameConstants::PLAYER_SPEED;
			float bombExploTime = node->properties.affectedByExplosion ? node->properties.timeTillExplosion : 2.f;
			bool goalAffectedByExplosion = node->properties.affectedByExplosion;

			if (!pathEngine->getGraph()->inLine<BlockComponent>(node->x, node->y, 7)) // TODO replace the range
				return false;

			pathEngine->getSimGraph()->placeBomb(node->x, node->y, 7, bombExploTime); // TODO replace the range

			bool blocksAffected = node->properties.numOfBlocksAffectedByExplosion > 0;
			bool itemsAffected = node->properties.numOfItemsAffectedByExplosion > 0;

			if (!blocksAffected || itemsAffected)
				found = false;

			if (found)
			{
				Path safePath;
				pathEngine->breadthFirstSearch(node->x, node->y, safePath, RateSafety(), taskNum + 1);

				if (safePath.nodeCount < 2 || !isSafePath(safePath))
					found = false;

				if (found && goalAffectedByExplosion)
				{
					// Check the full path to be sure if it's really safe
					Path fullPath;
					fullPath.attach(pathOut);
					fullPath.attach(safePath);

					if (!isSafePath(fullPath))
						found = false;
				}
			}

			if (found)
				pathOut.rating = node->properties.numOfBlocksAffectedByExplosion - timePerCell * pathOut.nodeCount;

			// Reset to the old state
			pathEngine->getSimGraph()->resetSimulation();

			if (found)
				return true;
		}
	}

	return false;
}
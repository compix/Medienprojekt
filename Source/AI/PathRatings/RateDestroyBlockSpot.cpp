#include "RateDestroyBlockSpot.h"
#include "../../GameConstants.h"
#include "../Checks/IsSafePath.h"
#include "../../Components/BlockComponent.h"
#include "RateSafety.h"
#include "../../Utils/PathFinding/SimulationGraph.h"
#include "../Checks/AIUtil.h"

RateDestroyBlockSpot::RateDestroyBlockSpot(entityx::Entity& entity)
	:m_entity(entity)
{
}

bool RateDestroyBlockSpot::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	IsSafePath isSafePath;

	if (node->valid)
	{
		pathEngine->makePath(pathOut, node, taskNum);
		if (isSafePath(m_entity, pathOut))
		{
			bool found = true;
			float timePerCell = AIUtil::getTimePerCell(m_entity);
			float bombExploTime = node->properties.affectedByExplosion ? node->properties.timeTillExplosion : 2.f;
			bool spotAffectedByExplosion = node->properties.affectedByExplosion;

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
				pathEngine->breadthFirstSearch(node->x, node->y, safePath, RateSafety(m_entity), taskNum + 1);

				if (safePath.nodeCount < 2 || !isSafePath(m_entity, safePath))
					found = false;

				if (found && spotAffectedByExplosion)
				{
					// Check the full path to be sure if it's really safe
					Path fullPath;
					fullPath.attach(pathOut);
					fullPath.attach(safePath);

					if (!isSafePath(m_entity, fullPath))
						found = false;
				}
			}

			if (found)
				pathOut.rating = node->properties.numOfBlocksAffectedByExplosion - timePerCell * pathOut.nodeCount;

			// Reset to the old state
			pathEngine->getSimGraph()->resetSimulation();

			return found;
		}
	}

	return false;
}
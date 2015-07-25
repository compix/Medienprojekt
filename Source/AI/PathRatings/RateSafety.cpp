#include "RateSafety.h"
#include "../Checks/IsSafePath.h"

bool RateSafety::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	if (!node->properties.affectedByExplosion)
	{
		pathEngine->makePath(pathOut, node, taskNum);
		// Check if the new path is better than the best known path
		float minExploTime;
		IsSafePath isSafePath;
		isSafePath(pathOut, &minExploTime);
		pathOut.rating = minExploTime;

		return true;
	}

	return false;
}
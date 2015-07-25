#include "RateDesperateSaveAttempt.h"
#include "../Checks/IsSafePath.h"

bool RateDesperateSaveAttempt::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	pathEngine->makePath(pathOut, node, taskNum);
	// Check if the new path is better than the best known path
	float minExploTime;
	IsSafePath isSafePath;
	if (!isSafePath(pathOut, &minExploTime))
	{		
		if (pathOut.nodes[0]->properties.affectedByExplosion && pathOut.nodes[0]->properties.timeTillExplosion > 0.2f)
			return false;

		pathOut.rating = node->properties.affectedByExplosion ? node->properties.timeTillExplosion : 3.f;
	}

	return true;
}
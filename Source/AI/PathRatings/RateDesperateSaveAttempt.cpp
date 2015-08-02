#include "RateDesperateSaveAttempt.h"
#include "../Checks/IsSafePath.h"

RateDesperateSaveAttempt::RateDesperateSaveAttempt(entityx::Entity& entity)
	:m_entity(entity)
{
}

bool RateDesperateSaveAttempt::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	pathEngine->makePath(pathOut, node, taskNum);

	float minExploTime;
	IsSafePath isSafePath;
	if (!isSafePath(m_entity, pathOut, &minExploTime))
	{		
		if (pathOut.nodes[0]->properties.affectedByExplosion && pathOut.nodes[0]->properties.timeTillExplosion > 0.2f)
			return false;

		pathOut.rating = node->properties.affectedByExplosion ? node->properties.timeTillExplosion : 3.f;
	}

	return true;
}
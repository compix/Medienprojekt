#include "RateDesperateSaveAttempt.h"
#include "../AIUtil.h"

bool RateDesperateSaveAttempt::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	float minExploTime;
	/*
	if (!isSafePath(m_entity, pathOut, &minExploTime))
	{		
		if (pathOut.nodes[0]->properties.affectedByExplosion && pathOut.nodes[0]->properties.timeTillExplosion > 0.2f)
			return false;

		pathOut.rating = node->properties.affectedByExplosion ? node->properties.timeTillExplosion : 3.f;
	}*/

	if (path.goal()->valid && AIUtil::isSafePath(entity, path, &minExploTime))
	{
		path.rating = path.goal()->properties.affectedByExplosion ? path.goal()->properties.timeTillExplosion : 3.f;
		return true;
	}

	return false;
}
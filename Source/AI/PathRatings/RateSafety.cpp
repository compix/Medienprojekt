#include "RateSafety.h"
#include "../AIUtil.h"

RateSafety::RateSafety(entityx::Entity& entity)
	:m_entity(entity)
{
}

bool RateSafety::operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum)
{
	if (!node->properties.affectedByExplosion)
	{
		pathEngine->makePath(pathOut, node, taskNum);

		float minExploTime;
		bool safe = AIUtil::isSafePath(m_entity, pathOut, &minExploTime);
		pathOut.rating = minExploTime + (safe ? 1.f : 0.f);

		return true;
	}

	return false;
}
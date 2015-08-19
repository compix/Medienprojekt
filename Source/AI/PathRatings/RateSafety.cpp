#include "RateSafety.h"
#include "../AIUtil.h"
#include "../../Utils/Math.h"

bool RateSafety::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	if (!path.goal()->valid)
		return false;

	float minExploTime;
	if (!path.goal()->properties.affectedByExplosion && AIUtil::isSafePath(entity, path, &minExploTime))
	{
		path.rating = Math::clamp(minExploTime, 0.f, 1.f);
		return true;
	}

	return false;
}
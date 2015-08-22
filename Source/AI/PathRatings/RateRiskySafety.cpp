#include "RateRiskySafety.h"
#include "../../Utils/Math.h"
#include "../AIUtil.h"

bool RateRiskySafety::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	if (!path.goal()->valid)
		return false;

	float minExploTime;
	if (!path.goal()->properties.affectedByExplosion)
	{
		float timePerCell = AIUtil::getTimePerCell(entity);
		AIUtil::isSafePath(entity, path, &minExploTime);
		if (minExploTime < timePerCell * 0.5f)
			return false;

		path.rating = Math::clamp(minExploTime, 0.f, 1.f);
		return true;
	}

	return false;
}
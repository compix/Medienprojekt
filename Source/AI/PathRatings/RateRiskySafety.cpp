#include "RateRiskySafety.h"
#include "../../Utils/Math.h"
#include "../AIUtil.h"
#include "../../Components/AIComponent.h"

bool RateRiskySafety::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	if (!path.goal()->valid)
		return false;

	float minExploTime;
	if (!path.goal()->properties.affectedByExplosion)
	{
		float timePerCell = AIUtil::getTimePerCell(entity);
		AIUtil::isSafePath(entity, path, &minExploTime);
		if (minExploTime < timePerCell * 0.9f)
			return false;

		auto& personality = entity.component<AIComponent>()->personality;
		auto& desires = personality.desires;
		auto& affinity = personality.affinity;
		path.rating = (affinity.getSafe + Math::clamp(minExploTime, 0.f, 1.f)) * desires.getSafe;
		return true;
	}

	return false;
}
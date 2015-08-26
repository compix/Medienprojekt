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
		if (minExploTime < timePerCell * 0.8f)
			return false;

		auto& personality = entity.component<AIComponent>()->personality;
		auto& desires = personality.desires;
		auto& affinity = personality.affinity;
		path.rating = (minExploTime / 5.f + affinity.getSafe - timePerCell * path.nodes.size() * 0.5f) * desires.getSafe;
		return true;
	}

	return false;
}
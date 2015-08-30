#include "RateRiskySafety.h"
#include "../../Utils/Math.h"
#include "../AIUtil.h"
#include "../../Components/AIComponent.h"

RateRiskySafety::RateRiskySafety()
	:m_exploTimeInfluence(0.2f), m_pathDurationInfluence(0.5f)
{
}

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
		path.rating = (minExploTime * m_exploTimeInfluence + affinity.getSafe - path.requiredTime(entity) * m_pathDurationInfluence) * desires.getSafe;
		return true;
	}

	return false;
}
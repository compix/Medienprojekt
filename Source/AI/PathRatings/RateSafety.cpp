#include "RateSafety.h"
#include "../AIUtil.h"
#include "../../Utils/Math.h"
#include "../../Components/AIComponent.h"

RateSafety::RateSafety()
	:m_exploTimeInfluence(0.2f), m_pathDurationInfluence(0.5f)
{
}

bool RateSafety::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	if (!path.goal()->valid)
		return false;

	float minExploTime;
	if (!path.goal()->properties.affectedByExplosion && AIUtil::isSafePath(entity, path, &minExploTime))
	{
		auto& personality = entity.component<AIComponent>()->personality;
		auto& desires = personality.desires;
		auto& affinity = personality.affinity;
		path.rating = (minExploTime * m_exploTimeInfluence + affinity.getSafe - path.requiredTime(entity) * m_pathDurationInfluence) * desires.getSafe;
		return true;
	}

	return false;
}
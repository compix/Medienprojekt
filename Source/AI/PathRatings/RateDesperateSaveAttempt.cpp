#include "RateDesperateSaveAttempt.h"
#include "../AIUtil.h"
#include "../../Components/AIComponent.h"

bool RateDesperateSaveAttempt::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	if (path.goal()->valid && AIUtil::isSafePath(entity, path))
	{
		auto& personality = entity.component<AIComponent>()->personality;
		auto& desires = personality.desires;
		auto& affinity = personality.affinity;
		path.rating = path.goal()->properties.affectedByExplosion ? path.goal()->properties.timeTillExplosion : affinity.getSafe;
		path.rating *= desires.getSafe;
		return true;
	}

	return false;
}
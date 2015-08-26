#include "RateSafety.h"
#include "../AIUtil.h"
#include "../../Utils/Math.h"
#include "../../Components/AIComponent.h"

bool RateSafety::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	if (!path.goal()->valid)
		return false;

	float minExploTime;
	if (!path.goal()->properties.affectedByExplosion && AIUtil::isSafePath(entity, path, &minExploTime))
	{
		float timePerCell = AIUtil::getTimePerCell(entity);
		auto& personality = entity.component<AIComponent>()->personality;
		auto& desires = personality.desires;
		auto& affinity = personality.affinity;
		path.rating = (minExploTime / 5.f + affinity.getSafe - timePerCell * path.nodes.size() * 0.5f) * desires.getSafe;
		return true;
	}

	return false;
}
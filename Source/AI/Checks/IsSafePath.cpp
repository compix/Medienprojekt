#include "IsSafePath.h"
#include "../../GameConstants.h"
#include "AIUtil.h"

bool IsSafePath::operator()(entityx::Entity& entity, Path& path, float* minExplosionTime)
{
	// How long does a player approx. need to get from one cell to another if the path is NOT blocked (worst case)?
	float timePerCell = AIUtil::getTimePerCell(entity);
	float updateTime = 1.f / 30.f;

	if (minExplosionTime)
		*minExplosionTime = 2.f;

	for (int i = 0; i < path.nodeCount; ++i)
	{
		auto node = path.nodes[i];
		if (node->properties.affectedByExplosion)
		{
			float exploTime = node->properties.timeTillExplosion - timePerCell * i;
			if (minExplosionTime)
				*minExplosionTime = std::min(*minExplosionTime, exploTime);
			if (exploTime >= -(timePerCell + updateTime * 2.f) && exploTime <= timePerCell + updateTime * 2.f)
				return false;
		}
	}

	return true;
}
#include "IsSafePath.h"
#include "../../GameConstants.h"

bool IsSafePath::operator()(Path& path, float* minExplosionTime)
{
	// How long does a player approx. need to get from one cell to another if the path is NOT blocked (worst case)?
	float timePerCell = (GameConstants::CELL_WIDTH / GameConstants::S_SCALE) / GameConstants::PLAYER_SPEED;

	if (minExplosionTime)
		*minExplosionTime = 3.f;

	for (int i = 0; i < path.nodeCount; ++i)
	{
		auto node = path.nodes[i];
		if (node->properties.affectedByExplosion)
		{
			float exploTime = node->properties.timeTillExplosion - timePerCell * i;
			if (minExplosionTime)
				*minExplosionTime = std::min(*minExplosionTime, exploTime);
			if (exploTime <= timePerCell)
				return false;
		}
	}

	return true;
}
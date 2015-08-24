#include "RateItem.h"
#include "../PathFinding/PathEngine.h"
#include "../AIUtil.h"
#include "RateSafety.h"
#include "../../Components/AIComponent.h"

bool RateItem::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();

	if (!goal->valid)
		return false;

	if (goal->properties.hasItem)
	{
		// TODO: Consider item rarity/value
		float timePerCell = AIUtil::getTimePerCell(entity);
		float minExploTime;

		// Check if the path is safe
		if (AIUtil::isSafePath(entity, path))
		{
			// Check if the path from that spot will be safe too by checking the full path
			AIPath safePath;
			pathEngine->searchBest(entity, goal->x, goal->y, safePath, RateSafety(), 1);

			AIPath fullPath;
			fullPath.attach(path);
			fullPath.attach(safePath);

			if (AIUtil::isSafePath(entity, fullPath, &minExploTime))
			{
				auto& personality = entity.component<AIComponent>()->personality;
				auto& desires = personality.desires;
				auto& affinity = personality.affinity;
				path.rating = affinity.getItem + minExploTime - path.nodes.size() * timePerCell;
				path.rating *= desires.getItem;
				return true;
			}			
		}
	}

	return false;
}
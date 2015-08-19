#include "RateItem.h"
#include "../../GameConstants.h"
#include "../PathFinding/PathEngine.h"
#include "../AIUtil.h"

bool RateItem::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	auto goal = path.goal();

	if (!goal->valid)
		return false;

	if (goal->properties.isItem)
	{
		// TODO: Consider item rarity/value
		float timePerCell = AIUtil::getTimePerCell(entity);
		float minExploTime;

		// Check if the path is safe
		if (AIUtil::isSafePath(entity, path))
		{
			// Check if the path from that spot will be safe too by checking the full path
			AIPath safePath;
			pathEngine->breadthFirstSearch(goal->x, goal->y, NodeType::SAFE, safePath, taskNum + 1);

			AIPath fullPath;
			fullPath.attach(path);
			fullPath.attach(safePath);

			if (AIUtil::isSafePath(entity, fullPath, &minExploTime))
			{
				path.rating = 3.f + minExploTime - path.nodes.size() * timePerCell;
				return true;
			}			
		}
	}

	return false;
}
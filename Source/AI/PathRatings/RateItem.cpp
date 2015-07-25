#include "RateItem.h"
#include "../../GameConstants.h"
#include "../Checks/IsSafePath.h"
#include "../../Utils/PathFinding/PathEngine.h"

bool RateItem::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	if (node->properties.isItem)
	{
		// TODO: Consider item rarity/value
		float timePerCell = (GameConstants::CELL_WIDTH / GameConstants::S_SCALE) / GameConstants::PLAYER_SPEED;
		pathEngine->makePath(pathOut, node, taskNum);
		float minExploTime;
		IsSafePath isSafePath;

		// Check if the path is safe
		if (isSafePath(pathOut))
		{
			// Check if the path from that spot will be safe too by checking the full path
			Path safePath;
			pathEngine->breadthFirstSearch(node->x, node->y, NodeType::SAFE, safePath, taskNum + 1);

			Path fullPath;
			fullPath.attach(pathOut);
			fullPath.attach(safePath);

			if (isSafePath(fullPath, &minExploTime))
			{
				pathOut.rating = minExploTime - pathOut.nodeCount * timePerCell;
				return true;
			}			
		}
	}

	return false;
}
#include "RateItem.h"
#include "../../GameConstants.h"
#include "../Checks/IsSafePath.h"
#include "../../Utils/PathFinding/PathEngine.h"
#include "../Checks/AIUtil.h"

RateItem::RateItem(entityx::Entity& entity)
	:m_entity(entity)
{
}

bool RateItem::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	if (node->properties.isItem)
	{
		// TODO: Consider item rarity/value
		float timePerCell = AIUtil::getTimePerCell(m_entity);
		pathEngine->makePath(pathOut, node, taskNum);
		float minExploTime;
		IsSafePath isSafePath;

		// Check if the path is safe
		if (isSafePath(m_entity, pathOut))
		{
			// Check if the path from that spot will be safe too by checking the full path
			Path safePath;
			pathEngine->breadthFirstSearch(node->x, node->y, NodeType::SAFE, safePath, taskNum + 1);

			Path fullPath;
			fullPath.attach(pathOut);
			fullPath.attach(safePath);

			if (isSafePath(m_entity, fullPath, &minExploTime))
			{
				pathOut.rating = 3.f + minExploTime - pathOut.nodeCount * timePerCell;
				return true;
			}			
		}
	}

	return false;
}
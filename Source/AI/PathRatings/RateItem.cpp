#include "RateItem.h"
#include "../../GameConstants.h"
#include "../PathFinding/PathEngine.h"
#include "../AIUtil.h"

RateItem::RateItem(entityx::Entity& entity)
	:m_entity(entity)
{
}

bool RateItem::operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum)
{
	if (node->properties.isItem)
	{
		// TODO: Consider item rarity/value
		float timePerCell = AIUtil::getTimePerCell(m_entity);
		pathEngine->makePath(pathOut, node, taskNum);
		float minExploTime;

		// Check if the path is safe
		if (AIUtil::isSafePath(m_entity, pathOut))
		{
			// Check if the path from that spot will be safe too by checking the full path
			AIPath safePath;
			pathEngine->breadthFirstSearch(node->x, node->y, NodeType::SAFE, safePath, taskNum + 1);

			AIPath fullPath;
			fullPath.attach(pathOut);
			fullPath.attach(safePath);

			if (AIUtil::isSafePath(m_entity, fullPath, &minExploTime))
			{
				pathOut.rating = 3.f + minExploTime - pathOut.nodes.size() * timePerCell;
				return true;
			}			
		}
	}

	return false;
}
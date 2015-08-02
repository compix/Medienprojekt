#include "RateDistanceToItems.h"
#include "../../Game.h"
#include "../../Components/CellComponent.h"

RateDistanceToItems::RateDistanceToItems(entityx::Entity& entity)
	:m_entity(entity)
{
}

bool RateDistanceToItems::operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum)
{
	float valueFactor = 3.f;
	pathOut.rating = inverseDistanceToItems(node) * valueFactor;
	return true;
}

float RateDistanceToItems::inverseDistanceToItems(GraphNode* node)
{
	float invDistance = 0;
	int itemCount = 0;
	for (auto& item : GameGlobals::entities->entities_with_components<ItemComponent>())
	{
		++itemCount;
		auto itemCell = item.component<CellComponent>();
		float distance = abs(node->x - itemCell->x) + abs(node->y - itemCell->y);

		if (distance == 0.f)
			continue;

		// Use the squared distance for a higher evaluation of close items and lower evaluation for items that are far away
		invDistance += 1.f / (distance * distance);
	}

	return invDistance / (itemCount > 0 ? itemCount : 1.f);
}
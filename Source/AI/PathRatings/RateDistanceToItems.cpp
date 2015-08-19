#include "RateDistanceToItems.h"
#include "../../Game.h"
#include "../../Components/CellComponent.h"

bool RateDistanceToItems::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	if (!path.goal()->valid)
		return false;

	float valueFactor = 3.f;
	path.rating = inverseDistanceToItems(path.goal()) * valueFactor;
	return true;
}

float RateDistanceToItems::inverseDistanceToItems(GraphNode* node)
{
	float invDistance = 0;
	int itemCount = 0;
	for (auto item : GameGlobals::entities->entities_with_components<ItemComponent>())
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
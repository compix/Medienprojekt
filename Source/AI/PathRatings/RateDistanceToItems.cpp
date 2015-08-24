#include "RateDistanceToItems.h"
#include "../../Game.h"
#include "../../Components/CellComponent.h"
#include "../../Components/AIComponent.h"

bool RateDistanceToItems::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	if (!path.goal()->valid)
		return false;

	auto& personality = entity.component<AIComponent>()->personality;
	auto& desires = personality.desires;
	auto& affinity = personality.affinity;
	path.rating = affinity.getItem + inverseDistanceToItems(path.goal());
	path.rating *= desires.getItem;
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
		int distance = abs(node->x - itemCell->x) + abs(node->y - itemCell->y);

		if (distance == 0.f)
			continue;

		// Use the squared distance for a higher evaluation of close items and lower evaluation for items that are far away
		invDistance += 1.f / (distance * distance);
	}

	return invDistance / (itemCount > 0 ? itemCount : 1.f);
}
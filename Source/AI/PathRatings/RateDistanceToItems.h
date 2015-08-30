#pragma once
#include "../PathFinding/PathEngine.h"

/**
* To get closer to items -> destroy blocks to make a path to get an item for example.
* Note: Use only in combination. It doesn't make sense on its own.
*/
class RateDistanceToItems
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

	float inverseDistanceToItems(GraphNode* node);
};
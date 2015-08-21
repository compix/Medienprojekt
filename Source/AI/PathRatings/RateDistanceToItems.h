#pragma once
#include "../PathFinding/PathEngine.h"

/**
* Use only in combination. It doesn't make sense on its own.
*/
class RateDistanceToItems
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum);

	float inverseDistanceToItems(GraphNode* node);
};
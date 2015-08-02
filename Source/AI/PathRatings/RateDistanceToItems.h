#pragma once
#include "../../Utils/PathFinding/PathEngine.h"

/**
* Use only in combination. It doesn't make sense on its own.
*/
class RateDistanceToItems
{
public:
	explicit RateDistanceToItems(entityx::Entity& entity);

	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);

	float inverseDistanceToItems(GraphNode* node);
private:
	entityx::Entity m_entity;
};
#pragma once
#include "../../Utils/PathFinding/PathEngine.h"

/**
* Use only in combination. It doesn't make sense on its own.
* ~Affected by explosion. RateDistanceToBlocksAffectedByExplosion is too long...
*/
class RateDistanceToAffectedBlocks
{
public:
	explicit RateDistanceToAffectedBlocks(entityx::Entity& entity);

	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);

	float inverseDistanceToAffectedBlocks(Graph* graph, GraphNode* node);
private:
	entityx::Entity m_entity;
};
#pragma once
#include "../PathFinding/PathEngine.h"

/**
* Use only in combination. It doesn't make sense on its own.
* ~Affected by explosion. RateDistanceToBlocksAffectedByExplosion is too long...
*/
class RateDistanceToAffectedBlocks
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

	float inverseDistanceToAffectedBlocks(Graph* graph, GraphNode* node);
};
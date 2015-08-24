#pragma once
#include "../PathFinding/PathEngine.h"

/**
* Considers blocks that are affected and about to be destroyed -> they can drop items.
*/
class RateDistanceToAffectedBlocks
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
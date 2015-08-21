#pragma once
#include "../PathFinding/PathEngine.h"

class RateDestroyBlockSpot
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
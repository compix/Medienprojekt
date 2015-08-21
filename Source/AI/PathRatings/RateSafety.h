#pragma once
#include "../PathFinding/PathEngine.h"

class RateSafety
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
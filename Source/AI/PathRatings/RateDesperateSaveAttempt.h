#pragma once
#include "../PathFinding/PathEngine.h"

/**
* It might make sense to just wait on a cell even if it is affected by an explosion in the simulation (of the future) but has a high time value.
* If another bomb explodes it might open a safe path.
*/
class RateDesperateSaveAttempt
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
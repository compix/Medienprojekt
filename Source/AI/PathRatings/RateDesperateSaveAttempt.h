#pragma once
#include "../PathFinding/PathEngine.h"

class RateDesperateSaveAttempt
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
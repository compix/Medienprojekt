#pragma once
#include "../PathFinding/PathEngine.h"

class RateAttackEnemy
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
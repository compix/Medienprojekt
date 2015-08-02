#pragma once

#include "../../Utils/PathFinding/PathEngine.h"

class IsSafePath
{
public:
	bool operator()(entityx::Entity& entity, Path& path, float* minExplosionTime = nullptr);
};
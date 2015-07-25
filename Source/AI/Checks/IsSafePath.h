#pragma once

#include "../../Utils/PathFinding/PathEngine.h"

class IsSafePath
{
public:
	bool operator()(Path& path, float* minExplosionTime = nullptr);
};
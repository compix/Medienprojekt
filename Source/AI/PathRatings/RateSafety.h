#pragma once
#include "../../Utils/PathFinding/PathEngine.h"

class RateSafety
{
public:
	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);
};
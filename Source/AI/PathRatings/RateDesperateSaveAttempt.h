#pragma once
#include "../../Utils/PathFinding/PathEngine.h"

class RateDesperateSaveAttempt
{
public:
	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);
};
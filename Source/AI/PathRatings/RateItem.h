#pragma once
#include <cinttypes>
#include "../../Utils/PathFinding/PathEngine.h"

struct GraphNode;
struct Path;

class RateItem
{
public:
	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);
};
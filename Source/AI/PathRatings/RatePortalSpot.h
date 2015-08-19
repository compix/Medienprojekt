#pragma once
#include <cinttypes>
#include "../PathFinding/PathEngine.h"

struct GraphNode;
struct AIPath;

class RatePortalSpot
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum);
};
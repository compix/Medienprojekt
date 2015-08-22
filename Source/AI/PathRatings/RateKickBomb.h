#pragma once
#include <cinttypes>
#include "../PathFinding/PathEngine.h"

struct GraphNode;
struct AIPath;

class RateKickBomb
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
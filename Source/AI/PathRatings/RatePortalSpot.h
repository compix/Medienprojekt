#pragma once
#include "../PathFinding/PathEngine.h"

struct GraphNode;
struct AIPath;

class RatePortalSpot
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
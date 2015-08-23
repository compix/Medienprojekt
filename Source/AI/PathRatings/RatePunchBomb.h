#pragma once
#include "../PathFinding/PathEngine.h"

struct GraphNode;
struct AIPath;

class RatePunchBomb
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
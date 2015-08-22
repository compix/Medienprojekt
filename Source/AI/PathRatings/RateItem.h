#pragma once
#include <cinttypes>
#include "../PathFinding/PathEngine.h"

struct GraphNode;
struct AIPath;

class RateItem
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
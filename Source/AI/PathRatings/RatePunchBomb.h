#pragma once
#include "../PathFinding/PathEngine.h"

struct GraphNode;
struct AIPath;

/**
* Used to get out of dangerous situations.
*/
class RatePunchBomb
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
};
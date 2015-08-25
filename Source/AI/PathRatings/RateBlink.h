#pragma once
#include "../PathFinding/PathEngine.h"

class RateBlink
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

private:
	bool isSafeLine(const GraphNode *const startNode, Graph* graph, Direction direction, float timeForPath);
};
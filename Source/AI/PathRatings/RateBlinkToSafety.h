#pragma once
#include "../PathFinding/PathEngine.h"

class RateBlinkToSafety
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

private:
	bool isSafeLine(GraphNode*const startNode, Graph* graph, Direction direction, float timeForPath);
};
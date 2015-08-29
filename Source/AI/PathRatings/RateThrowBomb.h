#pragma once
#include "../PathFinding/PathEngine.h"

class RateThrowBomb
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

private:
	GraphNode* getExpectedBombLanding(PathEngine* pathEngine, GraphNode* startNode, Direction direction, uint8_t& outThrowDistance);
};
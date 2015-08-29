#pragma once
#include "../PathFinding/PathEngine.h"

class RateBlink
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

private:
	bool isPromisingLine(GraphNode *const startNode, Graph* graph, Direction direction, float timeForPath, uint8_t& outNumItems, GraphNode** outEndNode);

private:
	std::vector<entityx::Entity> m_enemies;
};
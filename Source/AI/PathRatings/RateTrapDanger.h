#pragma once
#include <cinttypes>
#include <entityx/Entity.h>

class Graph;
class PathEngine;
struct GraphNode;
struct AIPath;

/**
* A character is considered trapped if there is no safe path out.
* This functor rates the risk of being trapped by placing bombs around the given node.
*
* NOTE: This rating only makes sense in combination.
*/
class RateTrapDanger
{
public:
	explicit RateTrapDanger(bool willPlaceBomb = false);

	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum);

private:
	int distanceToClosest(uint8_t x, uint8_t y, entityx::Entity& closestEnemy);

	bool testNode(GraphNode* startNode, GraphNode* testedNode, entityx::Entity& entity, PathEngine* pathEngine, uint8_t taskNum);
private:
	std::vector<entityx::Entity> m_enemies;

	bool m_willPlaceBomb;
};
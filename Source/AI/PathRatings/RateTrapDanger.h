#pragma once
#include <cinttypes>
#include <entityx/Entity.h>

class Graph;
class PathEngine;
struct GraphNode;
struct Path;

/**
* A character is considered trapped if there is no safe path out.
* This functor rates the risk of being trapped by placing bombs around the given node.
*
* NOTE: This rating only makes sense in combination.
*/
class RateTrapDanger
{
public:
	explicit RateTrapDanger(entityx::Entity self, std::vector<entityx::Entity>& enemies, bool willPlaceBomb = false);

	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);

private:
	int distanceToClosest(uint8_t x, uint8_t y);

	bool testNode(GraphNode* startNode, GraphNode* testedNode, PathEngine* pathEngine, uint8_t taskNum);
private:
	std::vector<entityx::Entity> m_enemies;
	entityx::Entity m_self;

	bool m_willPlaceBomb;
};
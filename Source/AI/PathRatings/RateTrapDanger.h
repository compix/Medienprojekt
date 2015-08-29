#pragma once
#include <entityx/Entity.h>
#include "../../Utils/Common.h"

class SimulationGraph;
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
	RateTrapDanger();

	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

private:
	bool isPotentialTrap(PathEngine* pathEngine, GraphNode* node, uint8_t range);
	bool downUpBlocked(SimulationGraph* graph, GraphNode* node);
	bool leftRightBlocked(SimulationGraph* graph, GraphNode* node);

	bool isPotentialTrap(SimulationGraph* graph, GraphNode* node, Direction direction, uint8_t range);
};
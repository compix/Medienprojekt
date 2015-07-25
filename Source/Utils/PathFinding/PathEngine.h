#pragma once
#include "Graph.h"
#include <memory>
#include "SimulationGraph.h"

struct Path
{
	Path() : nodeCount(0), cost(0), rating(-FLT_MAX) {}

	inline void attach(Path& path)
	{
		for (int i = 0; i < path.nodeCount; ++i)
			nodes.push_back(path.nodes[i]);

		nodeCount += path.nodeCount;
		cost += path.cost;
	}

	std::vector<GraphNode*> nodes;
	uint16_t nodeCount;
	uint16_t cost;

	// How good is the path? Higher values = better path
	float rating;
};

enum class NodeType
{
	SAFE,
	SAFE_NOT_MARKED,
	FREE, // Cell can be entered and is not marked
	ITEM,  // item which is NOT marked
	SPOT_FOR_BOMB
};

// Returns false if the path created with the goal is not valid
// Otherwise pathOut is set and rated
typedef std::function < bool(PathEngine* pathEngine, GraphNode* goal, Path& pathOut, uint8_t taskNum) > PathRatingFunction; // Second parameter is an output

class PathEngine
{
	struct NodePathInfo
	{
		NodePathInfo() : state(UNVISITED) {}

		enum State
		{
			UNVISITED,
			OPEN,
			CLOSED
		} state;
	};

public:
	PathEngine(LayerManager* layerManager);

	// Computes the path using the A* algorithm and stores it in pathOut.
	void computePath(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, Path& pathOut, uint8_t taskNum = 0);

	// Finds the shortest path to a node of the given type
	void breadthFirstSearch(uint8_t startX, uint8_t startY, NodeType targetType, Path& pathOut, uint8_t taskNum = 0);

	void breadthFirstSearch(uint8_t startX, uint8_t startY, Path& pathOut, PathRatingFunction ratePath, uint8_t taskNum = 0);

	void searchBest(uint8_t startX, uint8_t startY, Path& pathOut, PathRatingFunction ratePath, uint8_t maxChecks = 5, uint8_t taskNum = 0);

	void visualize();
	void visualize(Path& path);

	void update(float deltaTime);

	inline Graph* getGraph() const { return m_graph.get(); }
	inline SimulationGraph* getSimGraph() const { return m_simGraph.get(); }

	void makePath(Path& pathOut, GraphNode* goal, uint8_t taskNum);
private:
	uint32_t estimate(GraphNode* node, GraphNode* goal);

	// Inserts the node at the correct position starting at the given node to remain the cost order: lowest -> highest
	// This is fast because it's a grid and the costs are usually uniform. The open list is small. Updates/relaxations are rarely needed. 
	// Runtime: Usually O(1) - Theoretical worst case: O(n) where n is the number of nodes in the open list.
	void insert(GraphNode* newNode, GraphNode* at, uint8_t taskNum);
	// Runtime: O(1)
	GraphNode* remove(GraphNode* node, uint8_t taskNum);
private:
	std::unique_ptr<Graph> m_graph;
	std::unique_ptr<SimulationGraph> m_simGraph;

	LayerManager* m_layerManager;

	GraphNode m_startDummy, m_endDummy;
};
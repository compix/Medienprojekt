#pragma once
#include "Graph.h"
#include <memory>

const uint16_t MAX_PATH_LENGTH = 500;

struct Path
{
	Path() : nodeCount(0), cost(0) {}

	// Nodes from End to Start: End = 0 | Start = pathLength - 1
	GraphNode* nodes[MAX_PATH_LENGTH];
	uint16_t nodeCount;
	uint16_t cost;
};

class PathEngine
{
public:
	PathEngine(LayerManager* layerManager);

	// Computes the path using the A* algorithm and stores it in pathOut.
	void computePath(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, Path& pathOut);

	void visualize();
	void visualize(Path& path);

	inline Graph* getGraph() const { return m_graph.get(); }
private:
	uint32_t estimate(GraphNode* node, GraphNode* goal);

	// Inserts the node at the correct position starting at the given node to remain the cost order: lowest -> highest
	// This is fast because it's a grid and the costs are usually uniform. The open list is small. Updates/relaxations are rarely needed. 
	// Runtime: Usually O(1) - Theoretical worst case: O(n) where n is the number of nodes in the open list.
	void insert(GraphNode* newNode, GraphNode* at);
	// Runtime: O(1)
	GraphNode* remove(GraphNode* node);
	void makePath(Path& pathOut, GraphNode* goal);
private:
	std::unique_ptr<Graph> m_graph;
	LayerManager* m_layerManager;

	GraphNode m_startDummy, m_endDummy;
};
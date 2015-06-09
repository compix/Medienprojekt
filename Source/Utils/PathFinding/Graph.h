#pragma once
#include "GraphNode.h"
#include <vector>

class LayerManager;

class Graph
{
public:
	Graph(LayerManager* layerManager);

private:
	std::vector<GraphNode> m_nodes;
	LayerManager* m_layerManager;
};
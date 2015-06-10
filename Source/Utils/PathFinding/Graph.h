#pragma once
#include "GraphNode.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <stdint.h>
#include <assert.h>
#include <entityx/entityx.h>

class LayerManager;

class Graph
{
	friend class PathEngine;
public:
	Graph(LayerManager* layerManager);
	~Graph();

	void addNode(uint8_t x, uint8_t y);
	void removeNode(uint8_t x, uint8_t y);

	void onEntityAdded(entityx::Entity entity);
	void onEntityRemoved(entityx::Entity entity);

	void resetPathInfo();

	inline GraphNode* getNode(uint8_t x, uint8_t y) { assert(x > 0 && x < m_width - 1 && y > 0 && y < m_height - 1); return &m_nodeGrid[x][y]; };

	void init();

	void visualize();

	GraphNode* getNeighbor(GraphNode* node, const GraphNode::Neighbor& neighbor);
	bool hasNeighbor(GraphNode* node, GraphNode::Neighbor neighbor);
private:
	uint8_t m_width, m_height;
	GraphNode** m_nodeGrid;
	LayerManager* m_layerManager;
};
#pragma once
#include "GraphNode.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <stdint.h>
#include <assert.h>
#include "../../EntityLayer.h"
#include "../Common.h"

class LayerManager;

class Graph : public EntityLayer::IOnChangeListener
{
	friend class PathEngine;
public:
	Graph(LayerManager* layerManager);
	~Graph();

	void update();

	void addNode(uint8_t x, uint8_t y);
	void removeNode(uint8_t x, uint8_t y);

	void onEntityAdded(Entity *entity) override;
	void onEntityRemoved(Entity *entity) override;

	void resetPathInfo();

	inline GraphNode* getNode(uint8_t x, uint8_t y) { assert(x > 0 && x < m_width - 1 && y > 0 && y < m_height - 1); return &m_nodeGrid[x][y]; };

	void init();

	void visualize();

	GraphNode* getNeighbor(const GraphNode* node, const Direction& neighbor);
	bool hasNeighbor(const GraphNode* node, Direction neighbor);

	uint32_t estimateDangerCost(float remainingTimeTillExplosion);
private:
	void resetCosts();
private:
	uint8_t m_width, m_height;
	GraphNode** m_nodeGrid;
	LayerManager* m_layerManager;
};
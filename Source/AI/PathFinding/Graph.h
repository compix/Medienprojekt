#pragma once
#include "GraphNode.h"
#include <vector>
#include <stdint.h>
#include <entityx/entityx.h>
#include "../../EntityLayer.h"
#include "../../Utils/Common.h"
#include "../../LayerManager.h"
#include "../../GameConstants.h"

struct NormalBomb
{
	NormalBomb() : range(1), explosionTime(3.f), x(0), y(0) {}
	NormalBomb(uint8_t x, uint8_t y, uint8_t range, float explosionTime) : x(x), y(y), range(range), explosionTime(explosionTime) {}

	uint8_t range;
	float explosionTime;
	uint8_t x, y;
};

class Graph : public EntityLayer::IOnChangeListener
{
	friend class PathEngine;
	friend class AIVisualizer;
public:
	Graph(LayerManager* layerManager);
	virtual ~Graph();

	virtual void update(float deltaTime);

	void addNode(uint8_t x, uint8_t y);
	void removeNode(uint8_t x, uint8_t y);

	void onEntityAdded(entityx::Entity& entity) override;
	void onEntityRemoved(entityx::Entity& entity) override;

	inline GraphNode* getNode(uint8_t x, uint8_t y) { assert(x < m_width && y < m_height); return &m_nodeGrid[x][y]; }
	GraphNode* getNeighbor(const GraphNode* node, const Direction& neighbor);
	bool hasNeighbor(const GraphNode* node, Direction neighbor);
	GraphNode* getOtherPortalNode(uint8_t x, uint8_t y);

	virtual void placeBomb(uint8_t x, uint8_t y, uint8_t range, float explosionTime);

	template<class T>
	bool inLine(uint8_t x, uint8_t y, uint8_t range);

	void resetMarks();

	inline std::vector<entityx::Entity>& getAffectedBlocks() { return m_blocksAffectedByExplosion; }

	void reset();
protected:
	void explosionSpread(uint8_t x, uint8_t y, uint8_t range, float explosionTime, Direction direction);
	virtual void setOnFire(uint8_t x, uint8_t y, float explosionTime);

protected:
	uint8_t m_width, m_height;
	GraphNode** m_nodeGrid;
	LayerManager* m_layerManager;

	std::vector<NormalBomb> m_normalBombs;
	std::vector<entityx::Entity> m_blocksAffectedByExplosion;
};

template <class T>
bool Graph::inLine(uint8_t x, uint8_t y, uint8_t range)
{
	for (int i = 0; i < 4; ++i)
	{
		Direction direction = static_cast<Direction>(i);
		auto node = getNode(x, y);

		for (int j = 0; j <= range && node->x > 0 && node->x < m_width - 1 && node->y > 0 && node->y < m_height - 1; ++j)
		{
			if (m_layerManager->hasEntityWithComponent<T>(GameConstants::MAIN_LAYER, node->x, node->y))
				return true;

			node = getNeighbor(node, direction);
		}
	}
	
	return false;
}
#pragma once
#include "GraphNode.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <stdint.h>
#include <assert.h>
#include <entityx/entityx.h>
#include "../../EntityLayer.h"
#include "../Common.h"
#include <SFML/Window/Keyboard.hpp>
#include <queue>
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
public:
	Graph(LayerManager* layerManager);
	virtual ~Graph();

	virtual void update(float deltaTime);

	void addNode(uint8_t x, uint8_t y);
	void removeNode(uint8_t x, uint8_t y);

	void onEntityAdded(entityx::Entity& entity) override;
	void onEntityRemoved(entityx::Entity& entity) override;

	void resetPathInfo(uint8_t taskNum);

	inline GraphNode* getNode(uint8_t x, uint8_t y) { assert(x >= 0 && x <= m_width - 1 && y >= 0 && y <= m_height - 1); return &m_nodeGrid[x][y]; };

	void init();

	void visualize(bool nodes, bool smells);

	GraphNode* getNeighbor(const GraphNode* node, const Direction& neighbor);
	bool hasNeighbor(const GraphNode* node, Direction neighbor);

	virtual void placeBomb(uint8_t x, uint8_t y, uint8_t range, float explosionTime, float futureTime = 0.f);
	void removeBomb(uint8_t x, uint8_t y, uint8_t range);

	void timeTravel(float seconds, float t);

	template<class T>
	bool inLine(uint8_t x, uint8_t y, uint8_t range);

	void resetMarks();
protected:
	void resetCosts();
	void resetProperties();

	virtual void explosionSpread(uint8_t x, uint8_t y, uint8_t range, float explosionTime, Direction direction, float futureTime = 0.f);
protected:
	uint8_t m_width, m_height;
	GraphNode** m_nodeGrid;
	LayerManager* m_layerManager;

	std::vector<NormalBomb> m_normalBombs;
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
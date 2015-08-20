#pragma once
#include "GraphNode.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <stdint.h>
#include <assert.h>
#include <entityx/entityx.h>
#include "../../EntityLayer.h"
#include "../../Utils/Common.h"
#include <SFML/Window/Keyboard.hpp>
#include <queue>
#include "../../LayerManager.h"
#include "../../GameConstants.h"
#include <SFML/Graphics.hpp>

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

	inline GraphNode* getNode(uint8_t x, uint8_t y) { assert(x >= 0 && x <= m_width - 1 && y >= 0 && y <= m_height - 1); return &m_nodeGrid[x][y]; }

	void visualize(bool nodes, bool pathInfo, bool dangerZones, bool properties);

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
	void resetCosts();

	void explosionSpread(uint8_t x, uint8_t y, uint8_t range, float explosionTime, Direction direction);

	virtual void setOnFire(uint8_t x, uint8_t y, float explosionTime);

protected:
	uint8_t m_width, m_height;
	GraphNode** m_nodeGrid;
	LayerManager* m_layerManager;

	std::vector<NormalBomb> m_normalBombs;
	std::vector<entityx::Entity> m_blocksAffectedByExplosion;

private:
	// For visualization
	sf::Font m_font;
	sf::CircleShape m_circle;
	sf::RectangleShape m_rect;
	sf::Text m_text;
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
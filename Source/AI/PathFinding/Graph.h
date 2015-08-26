#pragma once
#include "GraphNode.h"
#include <vector>
#include <stdint.h>
#include <entityx/entityx.h>
#include "../../EntityLayer.h"
#include "../../Utils/Common.h"
#include "../../LayerManager.h"
#include "../../GameConstants.h"

struct AffectedByExplosion
{
	AffectedByExplosion() : numOfBlocks(0), numOfItems(0) {}

	uint8_t numOfBlocks;
	uint8_t numOfItems;
	std::vector<entityx::Entity> players;

	bool isEnemyAffected(const entityx::Entity& self)
	{
		for (auto& e : players)
			if (e != self)
				return true;
		return false;
	}
};

struct Bomb
{
	Bomb() : range(1), explosionTime(3.f), x(0), y(0) {}
	Bomb(uint8_t x, uint8_t y, uint8_t range, float explosionTime, bool ghost, bool lightning) 
		: x(x), y(y), range(range), explosionTime(explosionTime), ghost(ghost), lightning(lightning){}

	uint8_t x, y;
	uint8_t range;
	float explosionTime;
	bool ghost;
	bool lightning;
};

struct ExplosionSpread
{
	ExplosionSpread() : range(1), explosionTime(3.f), x(0), y(0) {}
	ExplosionSpread(uint8_t x, uint8_t y, uint8_t range, float explosionTime, Direction direction, bool ghost, bool lightning, bool lightningPeak)
		: x(x), y(y), range(range), explosionTime(explosionTime), direction(direction), ghost(ghost), lightning(lightning), lightningPeak(lightningPeak) {}

	uint8_t x, y;
	uint8_t range;
	float explosionTime;
	Direction direction;
	bool ghost;
	bool lightning;
	bool lightningPeak;
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
	GraphNode* getPortalNeighbor(uint8_t x, uint8_t y);

	virtual void placeBomb(const Bomb& bomb, AffectedByExplosion* affectedByExplosion = nullptr);

	template<class T>
	bool inLine(uint8_t x, uint8_t y, uint8_t range);

	void resetMarks();

	inline std::vector<entityx::Entity>& getAffectedBlocks() { return m_blocksAffectedByExplosion; }

	void reset();
protected:
	void explosionSpread(const ExplosionSpread& spread, AffectedByExplosion* affectedByExplosion = nullptr);
	virtual void setOnFire(uint8_t x, uint8_t y, float explosionTime);

	void spreadSmell(SmellType smellType, uint8_t startX, uint8_t startY, uint8_t range);

	inline bool inBounds(uint8_t x, uint8_t y) { return x < m_width && y < m_height; }
protected:
	uint8_t m_width, m_height;
	GraphNode** m_nodeGrid;
	LayerManager* m_layerManager;

	std::vector<Bomb> m_normalBombs;
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
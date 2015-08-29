#pragma once
#include <stdint.h>
#include <assert.h>
#include "../Components/InventoryComponent.h"

struct GraphNode;

struct BombProperties
{
	BombProperties(uint8_t explosionRange, float explosionTime, BombType type)
		: explosionSimulated(false), explosionRange(explosionRange), explosionTime(explosionTime), type(type) {}
	BombProperties() : BombProperties(explosionRange, 0.f, BombType::NORMAL) {}

	bool explosionSimulated;
	uint8_t explosionRange;
	BombType type;

	float explosionTime;
};

enum class SmellType
{
	DYING_BLOCK,
	ENEMY
};

struct Smells
{
	uint16_t dyingBlock; // <- Item could spawn.
	uint16_t enemy;
};

struct NodeProperties
{
	NodeProperties() : affectedByExplosion(false), timeTillExplosion(0.f), hasItem(false), 
		hasBomb(false), hasPlayer(false), hasPortal(false), hasBlock(false), otherPortal(nullptr), hasSolidBlock(false) {}

	float timeTillExplosion;

	GraphNode* otherPortal;

	bool hasItem;
	bool hasBomb;
	bool hasPlayer;
	bool hasPortal;
	bool hasBlock;
	bool hasSolidBlock;

	bool affectedByExplosion;
};

struct GraphNode
{
	GraphNode() : valid(true), marked(false) {}

	uint8_t x, y;
	bool valid;
	bool marked;

	NodeProperties properties;
	BombProperties bombProperties;
	Smells smells;

	uint16_t& smell(SmellType type)
	{
		switch (type)
		{
		case SmellType::DYING_BLOCK:
			return smells.dyingBlock;
		case SmellType::ENEMY:
			return smells.enemy;
		default:
			assert(false);
			return smells.dyingBlock;
		}
	}
};
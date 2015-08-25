#pragma once
#include <stdint.h>
#include <assert.h>

struct GraphNode;

struct BombProperties
{
	BombProperties(uint8_t explosionRange) : explosionSimulated(false), explosionRange(explosionRange) {}
	BombProperties() : BombProperties(explosionRange) {}

	bool explosionSimulated;
	uint8_t explosionRange;

	float explosionTime;
};

enum class SmellType
{
	DYING_BLOCK
};

struct Smells
{
	uint16_t dyingBlock; // <- Item could spawn.
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
		default:
			assert(false);
			return smells.dyingBlock;
		}
	}
};
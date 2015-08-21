#pragma once
#include <stdint.h>

struct GraphNode;

struct BombProperties
{
	BombProperties(uint8_t explosionRange) : explosionSimulated(false), explosionRange(explosionRange),
		numOfBlocksAffectedByExplosion(0), numOfPlayersAffectedByExplosion(0), numOfItemsAffectedByExplosion(0) {}
	BombProperties() : BombProperties(explosionRange) {}

	bool explosionSimulated;
	uint8_t explosionRange;

	// If there is a bomb on that node, how many destructible blocks will the explosion hit?
	uint8_t numOfBlocksAffectedByExplosion;

	// If there is a bomb on that node, how many players will the explosion hit?
	uint8_t numOfPlayersAffectedByExplosion;

	uint8_t numOfItemsAffectedByExplosion;
};

struct NodeProperties
{
	NodeProperties() : affectedByExplosion(false), timeTillExplosion(0.f), hasItem(false), 
		hasBomb(false), hasPlayer(false), hasPortal(false), hasBlock(false), otherPortal(nullptr) {}

	float timeTillExplosion;

	GraphNode* otherPortal;

	bool hasItem;
	bool hasBomb;
	bool hasPlayer;
	bool hasPortal;
	bool hasBlock;

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
};
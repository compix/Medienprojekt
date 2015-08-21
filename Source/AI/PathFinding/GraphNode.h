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
	GraphNode() : cost(1), valid(true), marked(false)
	{
		state[0] = UNVISITED;
		state[1] = UNVISITED;
	}

	uint8_t x, y;
	uint32_t cost;
	bool valid;

	// Path Information
	GraphNode* prevOnPath[2];

	uint32_t costSoFar;
	uint32_t estimatedTotalCost;

	bool marked;

	enum State
	{
		UNVISITED,
		OPEN,
		CLOSED
	} state[2];

	NodeProperties properties;
	BombProperties bombProperties;

	// List information to keep a sorted list.
	GraphNode* next[2];
	GraphNode* prev[2];
};
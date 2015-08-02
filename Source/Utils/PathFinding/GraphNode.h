#pragma once
#include <stdint.h>

struct NodeProperties
{
	NodeProperties() : affectedByExplosion(false), timeTillExplosion(0.f), 
		numOfBlocksAffectedByExplosion(0), numOfPlayersAffectedByExplosion(0), isItem(false), hasBomb(false) {}

	bool affectedByExplosion;
	float timeTillExplosion;

	// If there is a bomb on that node, how many destructible blocks will the explosion hit?
	uint8_t numOfBlocksAffectedByExplosion;

	// If there is a bomb on that node, how many players will the explosion hit?
	uint8_t numOfPlayersAffectedByExplosion;

	uint8_t numOfItemsAffectedByExplosion;

	bool isItem;
	bool hasBomb;
	bool hasPlayer;
	bool hasPortal;
};

struct GraphNode
{
	GraphNode() : cost(1), valid(false), marked(false) { }

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

	// List information to keep a sorted list.
	GraphNode* next[2];
	GraphNode* prev[2];
};
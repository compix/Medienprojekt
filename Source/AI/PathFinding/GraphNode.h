#pragma once
#include <stdint.h>

struct GraphNode;

struct BombProperties
{
	BombProperties(uint8_t explosionRange) : explosionSimulated(false), explosionRange(explosionRange) {}
	BombProperties() : BombProperties(explosionRange) {}

	bool explosionSimulated;
	uint8_t explosionRange;

	float explosionTime;
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
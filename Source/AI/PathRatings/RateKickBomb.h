#pragma once
#include <cinttypes>
#include "../PathFinding/PathEngine.h"

struct GraphNode;
struct AIPath;

class RateKickBomb
{
public:
	RateKickBomb(entityx::Entity& entity);

	bool operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum);

private:
	entityx::Entity m_entity;
};
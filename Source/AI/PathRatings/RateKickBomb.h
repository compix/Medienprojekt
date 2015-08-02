#pragma once
#include <cinttypes>
#include "../../Utils/PathFinding/PathEngine.h"

struct GraphNode;
struct Path;

class RateKickBomb
{
public:
	RateKickBomb(entityx::Entity& entity);

	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);

private:
	entityx::Entity m_entity;
};
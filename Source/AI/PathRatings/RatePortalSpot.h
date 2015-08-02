#pragma once
#include <cinttypes>
#include "../../Utils/PathFinding/PathEngine.h"

struct GraphNode;
struct Path;

class RatePortalSpot
{
public:
	RatePortalSpot(entityx::Entity& entity);

	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);

private:
	entityx::Entity m_entity;
};
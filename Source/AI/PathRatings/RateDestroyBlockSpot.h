#pragma once
#include "../PathFinding/PathEngine.h"

class RateDestroyBlockSpot
{
public:
	explicit RateDestroyBlockSpot(entityx::Entity& entity);

	bool operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum);

private:
	entityx::Entity m_entity;
};
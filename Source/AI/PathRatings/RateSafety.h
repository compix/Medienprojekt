#pragma once
#include "../../Utils/PathFinding/PathEngine.h"

class RateSafety
{
public:
	explicit RateSafety(entityx::Entity& entity);

	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);

private:
	entityx::Entity m_entity;
};
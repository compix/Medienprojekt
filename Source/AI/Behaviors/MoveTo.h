#pragma once
#include "Behavior.h"
#include <stdint.h>
#include "../PathFinding/PathEngine.h"
#include "FollowPath.h"

/**
* @brief Moves an entity to a given cell.
*/
class MoveTo : public FollowPath
{
public:
	MoveTo(PathEngine* pathEngine, Entity& entity, LayerManager* layerManager, uint8_t targetX, uint8_t targetY);

private:
	uint8_t m_targetX, m_targetY;

	PathEngine* m_pathEngine;
};
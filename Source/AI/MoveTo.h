#pragma once
#include "Behavior.h"
#include <stdint.h>
#include "../Utils/PathFinding/PathEngine.h"
#include "FollowPath.h"

/**
* @brief Moves an entity to a given cell.
*/
class MoveTo : public FollowPath
{
public:
	MoveTo(PathEngine* pathEngine, uint8_t targetX, uint8_t targetY);

	void prepare(entityx::Entity& entity) override;

private:
	uint8_t m_targetX, m_targetY;

	PathEngine* m_pathEngine;
};
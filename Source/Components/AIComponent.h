#pragma once
#include "../Systems/AISystem.h"
#include "../AI/PathFinding/PathEngine.h"

enum class BehaviorType
{
	PLACE_BOMB,

};

struct AIComponent
{
	AIComponent(uint8_t id) : id(id) {}

	AIAction action;
	AIPath path;
	uint8_t id;
};
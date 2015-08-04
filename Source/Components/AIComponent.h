#pragma once
#include "../Utils/PathFinding/PathEngine.h"
#include "../Systems/AISystem.h"

enum class BehaviorType
{
	PLACE_BOMB,

};

struct AIComponent
{
	AIComponent(uint8_t id) : id(id) {}

	AIAction action;
	Path path;
	uint8_t id;
};
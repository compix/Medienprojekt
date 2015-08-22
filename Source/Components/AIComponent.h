#pragma once
#include "../AI/Actions/ActionType.h"
#include "../AI/Actions/Action.h"

enum class BehaviorType
{
	PLACE_BOMB,

};

struct AIComponent
{
	AIComponent(uint8_t id) : id(id), currentAction(nullptr) {}

	std::unordered_map<ActionType, ActionPtr, EnumClassHash> actions;
	IAction* currentAction;
	ActionType currentActionType;
	uint8_t id;

	AIPath lastPath;
	ActionType lastActionType;
};
#pragma once
#include "../AI/Actions/ActionType.h"
#include "../AI/Actions/Action.h"
#include "../AI/Personality/AIPersonality.h"

enum class BehaviorType
{
	PLACE_BOMB,

};

struct AIComponent
{
    AIComponent(uint8_t id) : currentAction(nullptr), id(id) {}

	bool hasAction(ActionType type) { return actions.count(type) > 0; }

	std::unordered_map<ActionType, ActionPtr, EnumClassHash> actions;
	AbstractAction* currentAction;
	ActionType currentActionType;
	uint8_t id;

	AIPath lastPath;
	ActionType lastActionType;
	AIPersonality personality;

	// Used for some behavior like punch.
	GraphNode* behaviorNode;
};

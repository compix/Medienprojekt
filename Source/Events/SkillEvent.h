#pragma once
#include <entityx/Entity.h>

enum class SkillType : uint8_t;

struct SkillEvent
{
	SkillEvent(entityx::Entity triggerEntity, SkillType type, bool activate)
		: triggerEntity(triggerEntity), type(type), activate(activate) {}

	entityx::Entity triggerEntity;
	SkillType type;
	bool activate;
};

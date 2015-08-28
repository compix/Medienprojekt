#pragma once
#include <entityx/Entity.h>

using entityx::Entity;
struct JumpEvent
{
	JumpEvent(Entity entity)
		: entity(entity) {}

	Entity entity;
};
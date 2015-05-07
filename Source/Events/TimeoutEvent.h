#pragma once
#include <entityx/Entity.h>

struct TimeoutEvent
{
	TimeoutEvent(entityx::Entity affectedEntity)
		: affectedEntity(affectedEntity) {}

	entityx::Entity affectedEntity;
};
#pragma once
#include <entityx/Entity.h>

struct DeathEvent
{
	DeathEvent(entityx::Entity dyingEntity) : dyingEntity(dyingEntity) {}

	entityx::Entity dyingEntity;
};
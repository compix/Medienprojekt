#pragma once
#include <entityx/Entity.h>

struct DeathEvent
{
	DeathEvent(entityx::Entity dieingEntity) : dieingEntity(dieingEntity) {}

	entityx::Entity dieingEntity;
};
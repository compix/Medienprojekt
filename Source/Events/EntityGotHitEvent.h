#pragma once
#include <entityx/Entity.h>

struct EntityGotHitEvent
{
	EntityGotHitEvent(entityx::Entity damageDealer, entityx::Entity damagedEntity, int damage) 
		: damageDealer(damageDealer), damagedEntity(damagedEntity), damage(damage) {}

	entityx::Entity damageDealer;
	entityx::Entity damagedEntity;

	int damage;
};
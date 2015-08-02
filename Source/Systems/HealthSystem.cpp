#include "HealthSystem.h"

#include "../Components/HealthComponent.h"

#include <iostream>
#include "../GameGlobals.h"

HealthSystem::~HealthSystem()
{
	GameGlobals::events->unsubscribe<EntityGotHitEvent>(*this);
}

void HealthSystem::addedToEngine(Engine *engine)
{
	events.subscribe<EntityGotHitEvent>(*this);
}

void HealthSystem::update(float dt)
{
}

void HealthSystem::onEntityGotHit(Entity *damageDealer, Entity *damagedEntity, int damage)
{
	if (!damagedEntity->isValid() || !damageDealer->isValid())
		return;

	auto health = damagedEntity->get<HealthComponent>();

	if(health)
	{
		health->value -= damage;

		if (health->value <= 0)
		{
			GameGlobals::events->death.emit(damagedEntity); // Entity died
			damagedEntity.remove<HealthComponent>();
		}		
	}
}
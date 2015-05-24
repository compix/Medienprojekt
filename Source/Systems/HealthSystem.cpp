#include "HealthSystem.h"
#include "../Events/EntityGotHitEvent.h"
#include "../Components/HealthComponent.h"
#include "../Events/DeathEvent.h"
#include <iostream>
#include "../GameGlobals.h"

HealthSystem::~HealthSystem()
{
	GameGlobals::events->unsubscribe<EntityGotHitEvent>(*this);
}

void HealthSystem::configure(entityx::EventManager& events)
{
	events.subscribe<EntityGotHitEvent>(*this);
}

void HealthSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
}

void HealthSystem::receive(const EntityGotHitEvent& entityGotHit)
{
	if (!entityGotHit.damagedEntity.valid() || !entityGotHit.damageDealer.valid())
		return;

	entityx::Entity damagedEntity = entityGotHit.damagedEntity;

	auto health = damagedEntity.component<HealthComponent>();

	if(health)
	{
		health->value -= entityGotHit.damage;

		if (health->value <= 0)
		{
			GameGlobals::events->emit<DeathEvent>(damagedEntity); // Entity died
			damagedEntity.remove<HealthComponent>();
		}		
	}
}
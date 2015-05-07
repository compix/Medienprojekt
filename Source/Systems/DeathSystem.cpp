#include "DeathSystem.h"
#include "Events/DeathEvent.h"
#include "Components/DestructionDelayComponent.h"
#include <Components/DestructionComponent.h>

void DeathSystem::configure(entityx::EventManager& events)
{
	events.subscribe<DeathEvent>(*this);
}

void DeathSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
}

void DeathSystem::receive(const DeathEvent& deathEvent)
{
	auto dieingEntity = deathEvent.dieingEntity;

	if (!dieingEntity.valid())
		return;

	auto delayComponent = dieingEntity.component<DestructionDelayComponent>();

	if (delayComponent)
	{
		if (!dieingEntity.has_component<DestructionComponent>())
			dieingEntity.assign<DestructionComponent>(delayComponent->seconds);
	}
	else
	{
		if (!dieingEntity.has_component<DestructionComponent>())
			dieingEntity.assign<DestructionComponent>();
	}
}
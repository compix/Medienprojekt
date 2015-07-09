#include "DeathSystem.h"
#include "../Events/DeathEvent.h"
#include "../Components/DestructionDelayComponent.h"
#include "../Components/DestructionComponent.h"
#include "../Components/BodyComponent.h"
#include "../GameGlobals.h"

DeathSystem::~DeathSystem()
{
	GameGlobals::events->unsubscribe<DeathEvent>(*this);
}

void DeathSystem::configure(entityx::EventManager& events)
{
	events.subscribe<DeathEvent>(*this);
}

void DeathSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
}

void DeathSystem::receive(const DeathEvent& deathEvent)
{
	auto dyingEntity = deathEvent.dyingEntity;

	if (!dyingEntity.valid())
		return;

	auto delayComponent = dyingEntity.component<DestructionDelayComponent>();

	if (delayComponent)
	{
		if (!dyingEntity.has_component<DestructionComponent>())
			dyingEntity.assign<DestructionComponent>(delayComponent->seconds);
	}
	else
	{
		if (!dyingEntity.has_component<DestructionComponent>())
			dyingEntity.assign<DestructionComponent>();
	}
}
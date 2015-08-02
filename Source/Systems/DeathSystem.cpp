#include "DeathSystem.h"

#include "../Components/DestructionDelayComponent.h"
#include "../Components/DestructionComponent.h"
#include "../Components/BodyComponent.h"
#include "../GameGlobals.h"

DeathSystem::~DeathSystem()
{
	GameGlobals::events->unsubscribe<DeathEvent>(*this);
}

void DeathSystem::addedToEngine(Engine *engine)
{
	events.subscribe<DeathEvent>(*this);
}

void DeathSystem::update(float dt)
{
}

void DeathSystem::onDeath(Entity *dyingEntity)
{
	auto dyingEntity = deathEvent.dyingEntity;

	if (!dyingEntity->isValid())
		return;

	auto delayComponent = dyingEntity->get<DestructionDelayComponent>();

	if (delayComponent)
	{
		if (!dyingEntity->has<DestructionComponent>())
			dyingEntity.assign<DestructionComponent>(delayComponent->seconds);
	}
	else
	{
		if (!dyingEntity->has<DestructionComponent>())
			dyingEntity.assign<DestructionComponent>();
	}
}
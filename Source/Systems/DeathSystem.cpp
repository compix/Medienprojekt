#include "DeathSystem.h"
#include "../Components/DestructionDelayComponent.h"
#include "../Components/DestructionComponent.h"
#include "../Components/BodyComponent.h"
#include "../GameGlobals.h"

DeathSystem::~DeathSystem()
{
	m_connections.removeAll();
}

void DeathSystem::addedToEngine(Engine *engine)
{
	m_connections += GameGlobals::events->death.connect(this, &DeathSystem::onDeath);
}

void DeathSystem::update(float dt)
{
}

void DeathSystem::onDeath(Entity *dyingEntity)
{
	auto delayComponent = dyingEntity->get<DestructionDelayComponent>();

	if (delayComponent)
	{
		if (!dyingEntity->has<DestructionComponent>())
			dyingEntity->assign<DestructionComponent>(delayComponent->seconds);
	}
	else
	{
		if (!dyingEntity->has<DestructionComponent>())
			dyingEntity->assign<DestructionComponent>();
	}
}
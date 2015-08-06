#include "DestructionSystem.h"
#include "../Components/DestructionComponent.h"
#include "../Components/LinkComponent.h"
#include <ecstasy/core/Engine.h>

DestructionSystem::DestructionSystem()
: IteratingSystem(Family::all<DestructionComponent>().get()) {}

void DestructionSystem::processEntity(Entity *entity, float deltaTime)
{
	if (entity->isScheduledForRemoval())
		return;

	auto destructionComponent = entity->get<DestructionComponent>();

	destructionComponent->timeRemaining -= (float) deltaTime;

	if (destructionComponent->timeRemaining <= 0.f)
	{
		auto link = entity->get<LinkComponent>();

		if (link && link->dependent)
		{
			for (auto eId : link->links)
			{
				auto e = getEngine()->getEntity(eId);
				getEngine()->removeEntity(e);
			}
		}

		getEngine()->removeEntity(entity);
	}
}
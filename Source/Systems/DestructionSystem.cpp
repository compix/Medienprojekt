#include "DestructionSystem.h"
#include <Components/DestructionComponent.h>

void DestructionSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<DestructionComponent>())
	{
		if (!entity.valid())
			continue;

		auto destructionComponent = entity.component<DestructionComponent>();

		destructionComponent->timeRemaining -= dt;

		if (destructionComponent->timeRemaining <= 0.f)
		{
			entity.destroy();
		}
	}
}
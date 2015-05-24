#include "DestructionSystem.h"
#include "../Components/DestructionComponent.h"
#include "../Components/LinkComponent.h"

void DestructionSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<DestructionComponent>())
	{
		if (!entity.valid())
			continue;

		auto destructionComponent = entity.component<DestructionComponent>();

		destructionComponent->timeRemaining -= (float) dt;

		if (destructionComponent->timeRemaining <= 0.f)
		{
			auto link = entity.component<LinkComponent>();

			if (link && link->dependent)
			{
				for (auto e : link->links)
					e.destroy();
			}

			entity.destroy();
		}
	}
}
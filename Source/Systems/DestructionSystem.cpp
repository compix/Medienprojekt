#include "DestructionSystem.h"
#include "../Components/DestructionComponent.h"
#include "../Components/LinkComponent.h"

void DestructionSystem::update(float dt)
{
	for (auto entity : entityManager.entities_with_components<DestructionComponent>())
	{
		if (!entity->isValid())
			continue;

		auto destructionComponent = entity->get<DestructionComponent>();

		destructionComponent->timeRemaining -= (float) dt;

		if (destructionComponent->timeRemaining <= 0.f)
		{
			auto link = entity->get<LinkComponent>();

			if (link && link->dependent)
			{
				for (auto e : link->links)
					e.destroy();
			}

			entity->destroy();
		}
	}
}
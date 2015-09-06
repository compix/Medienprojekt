#include "DynamicPredictionSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/DynamicComponent.h"

using entityx::ComponentHandle;
using entityx::Entity;

void DynamicPredictionSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
    for (Entity entity : entityManager.entities_with_components<DynamicComponent, TransformComponent>())
	{
        auto dynamic = entity.component<DynamicComponent>();
        auto transform = entity.component<TransformComponent>();

		if (dynamic->updatedLastFrame)
			dynamic->updatedLastFrame = false;
		else
		{
			transform->x += static_cast<float>(dynamic->velX * dt);
			transform->y += static_cast<float>(dynamic->velY * dt);
		}
	}
}

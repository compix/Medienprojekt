#include "DynamicPredictionSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/DynamicComponent.h"

using entityx::ComponentHandle;
using entityx::Entity;

void DynamicPredictionSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	ComponentHandle<DynamicComponent> dynamic;
	ComponentHandle<TransformComponent> transform;

	for (Entity entity : entityManager.entities_with_components(dynamic, transform))
	{
		if (dynamic->updatedLastFrame)
			dynamic->updatedLastFrame = false;
		else
		{
			transform->x += static_cast<float>(dynamic->velX * dt);
			transform->y += static_cast<float>(dynamic->velY * dt);
		}
	}
}

#include "LightSystem.h"
#include "../Components/LightComponent.h"
#include "../GameGlobals.h"

void LightSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<LightComponent>())
	{
		auto lightComponent = entity.component<LightComponent>();

		GameGlobals::window->draw(lightComponent->light);
	}
}
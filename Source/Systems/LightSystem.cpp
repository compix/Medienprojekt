#include "LightSystem.h"
#include "../Components/LightComponent.h"
#include "../GameGlobals.h"

void LightSystem::update(float dt)
{
	for (auto entity : entityManager.entities_with_components<LightComponent>())
	{
		auto lightComponent = entity->get<LightComponent>();

		GameGlobals::window->draw(lightComponent->light);
	}
}
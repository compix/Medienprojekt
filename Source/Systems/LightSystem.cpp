#include "LightSystem.h"
#include "../Components/LightComponent.h"

LightSystem::LightSystem(sf::RenderWindow* window)
	:m_window(window)
{

}

void LightSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<LightComponent>())
	{
		auto lightComponent = entity.component<LightComponent>();

		m_window->draw(lightComponent->light);
	}
}
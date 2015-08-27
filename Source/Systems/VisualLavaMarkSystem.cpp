#include "VisualLavaMarkSystem.h"
#include "../Components/MarkedLavaSpotComponent.h"
#include "../Components/TimerComponent.h"
#include "../Components/SpriteComponent.h"

void VisualLavaMarkSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	for (auto entity : entities.entities_with_components<MarkedLavaSpotComponent, TimerComponent, SpriteComponent>())
	{
		auto& sprite = entity.component<SpriteComponent>()->sprite;
		auto timer = entity.component<TimerComponent>();

		if (timer->seconds <= 0.f)
			continue;

		sprite.setColor(sf::Color(255 - 100 * timer->seconds, 0, 0, 255 - 100 * timer->seconds));
	}
}
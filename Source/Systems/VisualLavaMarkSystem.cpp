#include "VisualLavaMarkSystem.h"
#include "../Components/MarkedLavaSpotComponent.h"
#include "../Components/TimerComponent.h"
#include "../Components/SpriteComponent.h"
#include "../SFMLDebugDraw.h"

void VisualLavaMarkSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	for (auto entity : entities.entities_with_components<MarkedLavaSpotComponent, TimerComponent, SpriteComponent>())
	{
		auto& sprite = entity.component<SpriteComponent>()->sprite;
		auto timer = entity.component<TimerComponent>();

		if (timer->seconds <= 0.f)
			continue;

		sf::Uint8 c = sf::Uint8(255.f / GameConstants::LAVA_SPAWN_TIME * timer->seconds);
		sprite.setColor(sf::Color(255, c, c));
	}
}
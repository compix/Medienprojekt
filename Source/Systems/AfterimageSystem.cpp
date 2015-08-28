#include "AfterimageSystem.h"
#include "../Components/AfterimageComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TimerComponent.h"
#include "../Events/TimeoutEvent.h"
#include "../Game.h"

AfterimageSystem::AfterimageSystem()
{

}

AfterimageSystem::~AfterimageSystem()
{
}

void AfterimageSystem::configure(entityx::EventManager &event_manager)
{
	event_manager.subscribe<TimeoutEvent>(*this);
}

void AfterimageSystem::receive(const TimeoutEvent& timeoutEvent)
{
	if (timeoutEvent.affectedEntity && timeoutEvent.affectedEntity.has_component<AfterimageComponent>())
	{
		auto entity = timeoutEvent.affectedEntity;
		entity.destroy();
	}
}

void AfterimageSystem::update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<AfterimageComponent, SpriteComponent, TimerComponent>())
	{
		auto afteri = entity.component<AfterimageComponent>();
		auto sprite = entity.component<SpriteComponent>();
		auto timer = entity.component<TimerComponent>();


		auto color = sprite->sprite.getColor();
		color.a = sf::Uint8((timer->seconds / afteri->totalTime) * 255.f);
		sprite->sprite.setColor(color);
	}
}
#include "TimerSystem.h"
#include <Components/TimerComponent.h>
#include <Events/TimeoutEvent.h>

void TimerSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<TimerComponent>())
	{
		if (!entity.valid())
			continue;

		auto timer = entity.component<TimerComponent>();
		
		if (timer->active)
		{
			timer->seconds -= dt;

			if (timer->seconds <= 0.f)
			{
				entity.remove<TimerComponent>();
				eventManager.emit<TimeoutEvent>(entity);
			}
		}
	}
}
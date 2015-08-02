#include "TimerSystem.h"
#include "../Components/TimerComponent.h"


void TimerSystem::update(float dt)
{
	for (auto entity : entityManager.entities_with_components<TimerComponent>())
	{
		if (!entity->isValid())
			continue;

		auto timer = entity->get<TimerComponent>();
		
		if (timer->active)
		{
			timer->seconds -= (float) dt;

			if (timer->seconds <= 0.f)
			{
				entity->remove<TimerComponent>();
				eventManager.emit<TimeoutEvent>(entity);
			}
		}
	}
}
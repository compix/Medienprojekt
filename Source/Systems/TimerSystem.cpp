#include "TimerSystem.h"
#include "../Components/TimerComponent.h"
#include "../GameGlobals.h"
#include <ecstasy/core/Engine.h>

TimerSystem::TimerSystem() 
: IteratingSystem(Family::all<TimerComponent>().get()) {}

void TimerSystem::processEntity(Entity *entity, float deltaTime)
{
	if (!entity->isValid())
		return;

	auto timer = entity->get<TimerComponent>();

	if (timer->active)
	{
		timer->seconds -= (float) deltaTime;

		if (timer->seconds <= 0.f)
		{
			entity->remove<TimerComponent>();
			GameGlobals::events->timeout.emit(entity);
		}
	}
}
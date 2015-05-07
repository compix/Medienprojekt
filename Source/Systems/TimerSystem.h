#pragma once
#include <entityx/System.h>

class TimerSystem : public entityx::System<TimerSystem>
{
public:
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;
};


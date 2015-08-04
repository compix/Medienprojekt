#pragma once
#include <ecstasy/systems/IteratingSystem.h>

class TimerSystem : public IteratingSystem<TimerSystem>
{
public:
	TimerSystem();
	void processEntity(Entity *entity, float deltaTime) override;
};


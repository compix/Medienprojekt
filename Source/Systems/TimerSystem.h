#pragma once
#include <ecstasy/core/EntitySystem.h>

class TimerSystem : public EntitySystem<TimerSystem>
{
public:
	void update(float dt) override;
};


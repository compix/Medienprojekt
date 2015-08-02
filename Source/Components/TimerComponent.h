#pragma once
#include <ecstasy/core/Component.h>

struct TimerComponent: public ECS::Component<TimerComponent>
{
	float seconds = 0;
	bool active = true;
	
	void reset() override {
		seconds = 0;
		active = true;
	}
};
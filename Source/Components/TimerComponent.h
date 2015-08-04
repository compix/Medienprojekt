#pragma once
#include <ecstasy/core/Component.h>

struct TimerComponent: public Component<TimerComponent>
{
	float seconds = 0;
	bool active = true;
	
	void reset() override {
		seconds = 0;
		active = true;
	}
};
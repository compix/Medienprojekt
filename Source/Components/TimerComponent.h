#pragma once
#include <ecstasy/core/Component.h>

struct TimerComponent: public Component<TimerComponent>
{
	TimerComponent(float seconds) : seconds(seconds), active(true) {}

	float seconds;
	bool active;
};
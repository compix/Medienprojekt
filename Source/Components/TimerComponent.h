#pragma once

struct TimerComponent
{
	TimerComponent(float seconds) : seconds(seconds), active(true) {}

	float seconds;
	bool active;
};
#pragma once
#include <ecstasy/core/Component.h>
#include <cinttypes>

struct PlayerComponent: public ECS::Component<PlayerComponent>
{
	uint8_t index = 0;
	
	void reset() override {
		index = 0;
	}
};
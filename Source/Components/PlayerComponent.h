#pragma once
#include <ecstasy/core/Component.h>
#include <cinttypes>

struct PlayerComponent: public Component<PlayerComponent>
{
	PlayerComponent(uint8_t index)
		: index(index) {}

	uint8_t index;
};
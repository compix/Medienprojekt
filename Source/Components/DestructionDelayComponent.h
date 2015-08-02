#pragma once
#include <ecstasy/core/Component.h>

/**
 * @brief	Delay the destruction of entities for the given time in seconds.
 */
struct DestructionDelayComponent: public ECS::Component<DestructionDelayComponent>
{
	float seconds = 0;
	
	void reset() override {
		seconds = 0;
	}
};
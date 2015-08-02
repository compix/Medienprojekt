#pragma once
#include <ecstasy/core/Component.h>

/**
 * @brief	An entity with this component will be destroyed after the given time in seconds.
 */
struct DestructionComponent: public ECS::Component<DestructionComponent>
{
	float timeRemaining = 0; // in seconds
	
	void reset() override {
		timeRemaining = 0;
	}
};
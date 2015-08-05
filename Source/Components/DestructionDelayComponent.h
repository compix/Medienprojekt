#pragma once
#include <ecstasy/core/Component.h>

/**
 * @brief	Delay the destruction of entities for the given time in seconds.
 */
struct DestructionDelayComponent: public Component<DestructionDelayComponent>
{
	DestructionDelayComponent(float seconds) : seconds(seconds) {}

	float seconds;
};
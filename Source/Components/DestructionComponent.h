#pragma once

/**
 * @brief	An entity with this component will be destroyed after the given time in seconds.
 */
struct DestructionComponent
{
	DestructionComponent() : timeRemaining(0.f) {}
	DestructionComponent(float timeRemaining) : timeRemaining(timeRemaining) {}

	float timeRemaining; // in seconds
};
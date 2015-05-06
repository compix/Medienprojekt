#pragma once

/**
 * @brief	Delay the destruction of entities for the given time in seconds.
 */
struct DestructionDelayComponent
{
	DestructionDelayComponent(float seconds) : seconds(seconds) {}

	float seconds;
};
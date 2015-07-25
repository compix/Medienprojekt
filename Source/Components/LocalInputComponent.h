#pragma once

/**
 * @brief	Serves as a flag.
 */
struct LocalInputComponent
{
	LocalInputComponent(int inputIndex) : inputIndex(inputIndex) {}
	int inputIndex;
};
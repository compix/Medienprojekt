#pragma once
#include <ecstasy/core/Component.h>

/**
 * @brief	Serves as a flag.
 */
struct LocalInputComponent: public Component<LocalInputComponent>
{
	LocalInputComponent(int inputIndex) : inputIndex(inputIndex) {}
	int inputIndex;
};
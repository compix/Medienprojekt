#pragma once
#include <ecstasy/core/Component.h>

/**
 * @brief	Serves as a flag.
 */
struct LocalInputComponent: public Component<LocalInputComponent>
{
	int inputIndex = 0;
	
	void reset() override {
		inputIndex = 0;
	}
};
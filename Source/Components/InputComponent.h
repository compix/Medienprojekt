#pragma once
#include <ecstasy/core/Component.h>

struct InputComponent: public Component<InputComponent>
{
	float moveX = 0;
	float moveY = 0;
	bool bombButtonPressed = false;
	bool skillButtonPressed = false;
	uint64_t packetNumber = 0; // for netcode
};
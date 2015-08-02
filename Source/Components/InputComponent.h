#pragma once
#include <ecstasy/core/Component.h>

struct InputComponent: public ECS::Component<InputComponent>
{
	float moveX = 0;
	float moveY = 0;
	bool bombButtonPressed = false;
	bool skillButtonPressed = false;
	uint64_t packetNumber = 0; // for netcode
	
	void reset() override {
		moveX = 0;
		moveY = 0;
		bombButtonPressed = false;
		skillButtonPressed = false;
		packetNumber = 0;
	}
};
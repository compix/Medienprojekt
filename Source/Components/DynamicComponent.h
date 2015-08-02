#pragma once
#include <ecstasy/core/Component.h>

// Marks entities as dynamic: they move around
struct DynamicComponent: public ECS::Component<DynamicComponent>
{
	uint64_t packetNumber = 0; // for netcode
	
	void reset() override {
		packetNumber = 0;
	}
};
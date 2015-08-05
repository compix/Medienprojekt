#pragma once
#include <ecstasy/core/Component.h>

// Marks entities as dynamic: they move around
struct DynamicComponent: public Component<DynamicComponent>
{
	uint64_t packetNumber = 0; // for netcode
};
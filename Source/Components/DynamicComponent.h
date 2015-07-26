#pragma once

// Marks entities as dynamic: they move around
struct DynamicComponent
{
	uint64_t packetNumber = 0; // for netcode
};
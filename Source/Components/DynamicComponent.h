#pragma once

// Marks entities as dynamic: they move around
struct DynamicComponent
{
	uint64_t packetNumber = 0; // for netcode
	float velX = 0;
	float velY = 0;
};
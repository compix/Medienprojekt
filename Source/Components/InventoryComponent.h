#pragma once
#include "entityx/entityx.h"

struct InventoryComponent
{
	InventoryComponent() : bombCount(8), bombKick(true), speedMultiplicator(1){}

	int bombCount;
	float speedMultiplicator;
	bool bombKick;

	std::pair<entityx::Entity, entityx::Entity> placedPortals;
};
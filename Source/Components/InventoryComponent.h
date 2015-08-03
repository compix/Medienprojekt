#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics/Color.hpp>

struct InventoryComponent
{
	InventoryComponent() : bombCount(8), bombKick(true), speedMultiplicator(1){}

	int bombCount;
	float speedMultiplicator;
	bool bombKick;

	std::pair<entityx::Entity, entityx::Entity> placedPortals;
};
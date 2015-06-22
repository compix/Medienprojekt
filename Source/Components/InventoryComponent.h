#pragma once

struct InventoryComponent
{
	InventoryComponent() : bombCount(8), bombKick(false), speedMultiplicator(1){}

	int bombCount;
	float speedMultiplicator;
	bool bombKick;
};
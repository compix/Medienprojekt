#pragma once

struct InventoryComponent
{
	InventoryComponent() : bombCount(8), bombKick(false){}

	int bombCount;
	bool bombKick;
};
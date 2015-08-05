#pragma once
#include <ecstasy/core/Component.h>

struct InventoryComponent: public Component<InventoryComponent>
{
	InventoryComponent() : bombCount(8), bombKick(false), speedMultiplicator(1){}

	int bombCount;
	float speedMultiplicator;
	bool bombKick;
};
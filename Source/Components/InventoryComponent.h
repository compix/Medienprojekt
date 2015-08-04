#pragma once
#include <ecstasy/core/Component.h>

struct InventoryComponent: public Component<InventoryComponent>
{
	int bombCount = 8;
	float speedMultiplicator = 1;
	bool bombKick = false;
	
	void reset() override {
		bombCount = 8;
		speedMultiplicator = 1;
		bombKick = false;
	}
};
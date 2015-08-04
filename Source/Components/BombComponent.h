#pragma once
#include <ecstasy/core/Component.h>

struct BombComponent: public Component<BombComponent>
{
	int explosionRange = 0;
	float explosionSpreadTime = 0; // in seconds
	bool exploded = false;
	
	void reset() override {
		explosionRange = 0;
		explosionSpreadTime = 0;
		exploded = false;
	}
};
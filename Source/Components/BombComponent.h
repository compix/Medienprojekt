#pragma once
#include <ecstasy/core/Component.h>

struct BombComponent: public Component<BombComponent>
{
	BombComponent(int explosionRange, float explosionSpreadTime) 
		: explosionRange(explosionRange), explosionSpreadTime(explosionSpreadTime), exploded(false) {}

	int explosionRange;
	float explosionSpreadTime; // in seconds
	bool exploded;
};
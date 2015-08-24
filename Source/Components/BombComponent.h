#pragma once

struct BombComponent
{
	BombComponent(int explosionRange, float explosionSpreadTime, bool ghost) 
		: explosionRange(explosionRange), explosionSpreadTime(explosionSpreadTime), exploded(false), ghost(ghost) {}

	int explosionRange;
	float explosionSpreadTime; // in seconds
	bool exploded;
	bool ghost; // Explosion will only be stopped by solid blocks
};
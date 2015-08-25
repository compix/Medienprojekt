#pragma once

struct BombComponent
{
	BombComponent(int explosionRange, float explosionSpreadTime, bool ghost, bool lightning) 
		: explosionRange(explosionRange), explosionSpreadTime(explosionSpreadTime), exploded(false), ghost(ghost), lightning(lightning) {}

	int explosionRange;
	float explosionSpreadTime; // in seconds
	bool exploded;
	bool ghost; // Explosion will only be stopped by solid blocks
	bool lightning; // 1 range explosion when it is stopped
};
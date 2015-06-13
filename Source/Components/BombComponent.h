#pragma once

struct BombComponent
{
	BombComponent(int explosionRange, float explosionSpreadTime) 
		: explosionRange(explosionRange), explosionSpreadTime(explosionSpreadTime), exploded(false) {}

	int explosionRange;
	float explosionSpreadTime; // in seconds
	bool exploded;
};
#pragma once

struct BombComponent
{
	BombComponent(int explosionRange, float explosionSpreadTime) 
		: explosionRange(explosionRange), explosionSpreadTime(explosionSpreadTime) {}

	int explosionRange;
	float explosionSpreadTime; // in seconds
};
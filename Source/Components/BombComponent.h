#pragma once

enum class BombType : uint8_t;

struct BombComponent
{
	BombComponent(int explosionRange, float explosionSpreadTime, BombType type)
		: explosionRange(explosionRange), explosionSpreadTime(explosionSpreadTime), exploded(false), type(type) {}

	int explosionRange;
	float explosionSpreadTime; // in seconds
	bool exploded;
	BombType type;
};
#pragma once

struct TransformComponent
{
	TransformComponent(float x = 0.f, float y = 0.f, float rotation = 0.f, float scale = 1.f) : x(x), y(y), rotation(rotation), scale(scale) {}

	float x, y;
	float rotation;
	float scale;
};
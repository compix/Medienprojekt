#pragma once
#include <ecstasy/core/Component.h>

struct TransformComponent: public Component<TransformComponent>
{
	TransformComponent(float x = 0.f, float y = 0.f, float rotation = 0.f, float scaleX = 1.f, float scaleY = 1.f) 
		: x(x), y(y), rotation(rotation), scaleX(scaleX), scaleY(scaleY) {}

	float x, y;
	float rotation;
	float scaleX, scaleY;
};
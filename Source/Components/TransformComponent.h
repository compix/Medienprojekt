#pragma once
#include <ecstasy/core/Component.h>

struct TransformComponent: public ECS::Component<TransformComponent>
{
	float x = 0, y = 0;
	float rotation = 0;
	float scaleX = 1, scaleY = 1;
	
	void reset() override {
		x = y = 0;
		rotation = 0;
		scaleX = scaleY = 1;
	}
};
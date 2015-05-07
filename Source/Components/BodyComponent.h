#pragma once
#include "Box2D/Box2D.h"

struct BodyComponent
{

	float xOffset = 0.f;
	float yOffset = 0.f;
	b2Body* body;
};
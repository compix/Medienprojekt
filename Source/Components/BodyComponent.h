#pragma once
#include <ecstasy/core/Component.h>
#include "Box2D/Box2D.h"

struct BodyComponent: public Component<BodyComponent>
{
	b2Body* body;

	BodyComponent(b2Body* body) : body(body) {}

	~BodyComponent()
	{
		body->GetWorld()->DestroyBody(body);
	}
};
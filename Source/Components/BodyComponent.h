#pragma once
#include <ecstasy/core/Component.h>
#include "Box2D/Box2D.h"

struct BodyComponent: public ECS::Component<BodyComponent>
{
	b2Body* body = nullptr;
	
	void reset() override {
		if(body) {
			body->GetWorld()->DestroyBody(body);
			body = nullptr;
		}
	}
};
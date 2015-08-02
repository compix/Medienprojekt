#pragma once
#include <ecstasy/core/Component.h>

struct HealthComponent: public ECS::Component<HealthComponent>
{
	int value = 0;
	
	void reset() override {
		value = 0;
	}
};
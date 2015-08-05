#pragma once
#include <ecstasy/core/Component.h>

struct HealthComponent: public Component<HealthComponent>
{
	HealthComponent(int value) : value(value) {}

	int value;
};
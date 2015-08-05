#pragma once
#include <ecstasy/core/Component.h>
#include "../Lighting/Light.h"

struct LightComponent: public Component<LightComponent>
{
	LightComponent(sf::Vector2f center, sf::Color color, float radius, float degree, float rotation)
	{
		light.create(center, color, radius, degree, rotation);
	}

	Light light;
};
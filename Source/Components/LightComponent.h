#pragma once
#include <ecstasy/core/Component.h>
#include "../Lighting/Light.h"

struct LightComponent: public ECS::Component<LightComponent>
{
	Light light;
	
	void init(sf::Vector2f center, sf::Color color, float radius, float degree, float rotation)
	{
		light.create(center, color, radius, degree, rotation);
	}
};
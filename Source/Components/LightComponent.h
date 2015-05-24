#pragma once
#include "../Lighting/Light.h"

struct LightComponent
{
	LightComponent(sf::Vector2f center, sf::Color color, float radius, float degree, float rotation)
	{
		light.create(center, color, radius, degree, rotation);
	}

	Light light;
};
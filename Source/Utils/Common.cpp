#include "Common.h"

void Math::rotate(sf::Vector2f& p, float angle)
{
	float x = p.x;
	float y = p.y;

	p.x = x*cosf(angle) - y*sinf(angle);
	p.y = y*cosf(angle) + x*sinf(angle);
}
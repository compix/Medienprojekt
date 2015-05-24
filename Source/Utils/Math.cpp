#include "Math.h"
#include "Colors.h"

void Math::rotate(sf::Vector2f& p, const float& angle)
{
	const float x = p.x;
	const float c = cosf(angle);
	const float s = sinf(angle);

	p.x = x*c - p.y*s;
	p.y = p.y*c + x*s;
}

RGB Math::lerp(const RGB& start, const RGB& end, float t)
{
	RGB result;
	result.r = Math::lerp(start.r, end.r, t);
	result.g = Math::lerp(start.g, end.g, t);
	result.b = Math::lerp(start.b, end.b, t);

	return result;
}

HSV Math::lerp(const HSV& start, const HSV& end, float t)
{
	HSV result;
	result.h = Math::lerp(start.h, end.h, t);
	result.s = Math::lerp(start.s, end.s, t);
	result.v = Math::lerp(start.v, end.v, t);

	return result;
}

sf::Vector2f Math::lerp(const sf::Vector2f& start, const sf::Vector2f& end, float t)
{
	sf::Vector2f result;
	result.x = Math::lerp(start.x, end.x, t);
	result.y = Math::lerp(start.y, end.y, t);

	return result;
}

RGB Math::smoothstep(RGB start, RGB end, float t)
{
	RGB result;
	float m = t*t*(3 - 2 * t);
	result.r = start.r + m*(end.r - start.r);
	result.g = start.g + m*(end.g - start.g);
	result.b = start.b + m*(end.b - start.b);

	return result;
}

HSV Math::smoothstep(HSV start, HSV end, float t)
{
	HSV result;
	float m = t*t*(3 - 2 * t);
	result.h = start.h + m*(end.h - start.h);
	result.s = start.s + m*(end.s - start.s);
	result.v = start.v + m*(end.v - start.v);

	return result;
}

sf::Vector2f Math::smoothstep(const sf::Vector2f& start, const sf::Vector2f& end, float t)
{
	sf::Vector2f result;
	float m = t*t*(3 - 2 * t);
	result.x = start.x + m*(end.x - start.x);
	result.y = start.y + m*(end.y - start.y);

	return result;
}

RGB Math::regress(RGB start, RGB end, float t)
{
	RGB result;
	float m = sqrtf(t)*2.f - t;
	result.r = start.r + m*(end.r - start.r);
	result.g = start.g + m*(end.g - start.g);
	result.b = start.b + m*(end.b - start.b);

	return result;
}

HSV Math::regress(HSV start, HSV end, float t)
{
	HSV result;
	float m = sqrtf(t)*2.f - t;
	result.h = start.h + m*(end.h - start.h);
	result.s = start.s + m*(end.s - start.s);
	result.v = start.v + m*(end.v - start.v);

	return result;
}

sf::Vector2f Math::regress(const sf::Vector2f& start, const sf::Vector2f& end, float t)
{
	sf::Vector2f result;
	float m = sqrtf(t)*2.f - t;
	result.x = start.x + m*(end.x - start.x);
	result.y = start.y + m*(end.y - start.y);

	return result;
}
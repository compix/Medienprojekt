#pragma once
#include <algorithm>
#include <SFML/System/Vector2.hpp>

struct HSV;
struct RGB;

namespace Math
{
	const float PI = 3.14159265359f;
	const float EPSILON = 0.0000001f;

	void rotate(sf::Vector2f& p, const float&  angle);
	inline float lerp(float start, float end, float t) { return (1 - t)*start + t*end; }
	RGB lerp(const RGB& start, const RGB& end, float t);
	HSV lerp(const HSV& start, const HSV& end, float t);
	sf::Vector2f lerp(const sf::Vector2f& start, const sf::Vector2f& end, float t);

	inline float clamp(float v, float min, float max) { return v < min ? min : (v > max ? max : v); }

	inline float smoothstep(float start, float end, float t) { return start + t*t*(3 - 2 * t) * (end - start); }
	RGB smoothstep(RGB start, RGB end, float t);
	HSV smoothstep(HSV start, HSV end, float t);
	sf::Vector2f smoothstep(const sf::Vector2f& start, const sf::Vector2f& end, float t);

	inline float smootherstep(float start, float end, float t) { return start + t*t*t*(t*(t * 6 - 15) + 10) * (end-start); }

	inline float regress(float start, float end, float t) { return start + (sqrtf(t)*2.f - t)*(end-start); }
	RGB regress(RGB start, RGB end, float t);
	HSV regress(HSV start, HSV end, float t);
	sf::Vector2f regress(const sf::Vector2f& start, const sf::Vector2f& end, float t);

	inline float min(float v0, float v1, float v2) { return std::min(v0, std::min(v1, v2)); }
	inline float max(float v0, float v1, float v2) { return std::max(v0, std::max(v1, v2)); }

	inline float toRadians(float degree) { return degree * PI / 180.f; }
	inline float toDegrees(float radians) { return radians * 180.f / PI; }

	inline bool nearEq(float n1, float n2) { return std::abs(n1 - n2) <= EPSILON; }
}
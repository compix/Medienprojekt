#pragma once
#include <Graphics/ParticleEmitter.h>

enum class Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

namespace Math
{
	const float PI = 3.14159265359f;

	void rotate(sf::Vector2f& p, float angle);
	inline float lerp(float start, float end, float t) { return (1 - t)*start + t*end; }
	inline float clamp(float v, float min, float max) { return v < min ? min : v > max ? max : v; }
	inline float smoothstep(float start, float end, float t)
	{
		t = clamp((t - start) / (end - start), 0.f, 1.f);
		return t*t*(3 - 2 * t);
	}

	inline float smootherstep(float start, float end, float t)
	{
		t = clamp((t - start) / (end - start), 0.f, 1.f);
		return t*t*t*(t*(t * 6 - 15) + 10);
	}

	inline float regress(float t) { return sqrtf(t)*2.f - t; }
}

namespace Convert
{
	struct HSV
	{
		HSV() {}
		HSV(float h, float s, float v) : h(h), s(s), v(v) {}

		float h;       // angle in degrees
		float s;       // percent
		float v;       // percent
	};
}
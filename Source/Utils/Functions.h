#pragma once
#include "Colors.h"
#include "Math.h"

enum class GradientType
{
	LINEAR,
	REGRESS,
	SMOOTH
};

template<class T>
struct Gradient
{
	Gradient(GradientType type, const T& start, const T& end) : type(type), start(start), end(end) {}
	T operator()(float t)
	{
		switch (type)
		{
		case GradientType::LINEAR:
			return Math::lerp(start, end, t);
		case GradientType::REGRESS:
			return Math::regress(start, end, t);
		case GradientType::SMOOTH:
			return Math::smoothstep(start, end, t);
		}

		return T();
	}

	GradientType type;
	const T start;
	const T end;
};

template<class T>
struct Gradient3
{
	Gradient3(GradientType type, const T& start, const T& mid, const T& end) : type(type), mid(mid), start(start), end(end) {}
	T operator()(float t)
	{
		const T* s;
		const T* e;

		if (t < 0.5f)
		{
			s = &start;
			e = &mid;

			// normalize to [0, 1]
			t *= 2.f;
		}
		else
		{
			s = &mid;
			e = &end;

			// normalize to [0, 1]
			t = (t - 0.5f)*2.f;
		}

		switch (type)
		{
		case GradientType::LINEAR:
			return Math::lerp(*s, *e, t);
		case GradientType::REGRESS:
			return Math::regress(*s, *e, t);
		case GradientType::SMOOTH:
			return Math::smoothstep(*s, *e, t);
		}

		return T();
	}

	GradientType type;
	const T start;
	const T mid;
	const T end;
};
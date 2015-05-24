#include "Colors.h"
#include "Math.h"

HSV Convert::RGBtoHSV(RGB rgb)
{
	rgb.r /= 255.f;
	rgb.g /= 255.f;
	rgb.b /= 255.f;

	HSV hsv;
	float min, max, delta;
	min = Math::min(rgb.r, rgb.g, rgb.b);
	max = Math::max(rgb.r, rgb.g, rgb.b);
	hsv.v = max;								// v
	delta = max - min;
	if (max != 0)
		hsv.s = delta / max;					// s
	else
	{
		// r = g = b = 0					    // s = 0, v is undefined
		hsv.s = 0;
		hsv.h = -1;

		return hsv;
	}

	if (rgb.r == max)
		hsv.h = (rgb.g - rgb.b) / delta;		// between yellow & magenta
	else if (rgb.g == max)
		hsv.h = 2 + (rgb.b - rgb.r) / delta;	// between cyan & yellow
	else
		hsv.h = 4 + (rgb.r - rgb.g) / delta;	// between magenta & cyan
	hsv.h *= 60;							    // degrees

	if (hsv.h < 0)
		hsv.h += 360;

	hsv.s *= 100.f;
	hsv.v *= 100.f;
	return hsv;
}

RGB Convert::HSVtoRGB(HSV hsv)
{
	hsv.v /= 100.f;
	hsv.s /= 100.f;

	RGB rgb;

	int i;
	float f, p, q, t;
	if (hsv.s == 0)
	{
		// achromatic (grey)
		rgb.r = rgb.g = rgb.b = hsv.v * 255.f;
		return rgb;
	}

	hsv.h /= 60;			// sector 0 to 5
	i = floor(hsv.h);
	f = hsv.h - i;			// factorial part of h
	p = hsv.v * (1 - hsv.s);
	q = hsv.v * (1 - hsv.s * f);
	t = hsv.v * (1 - hsv.s * (1 - f));

	switch (i)
	{
	case 0:
		rgb.r = hsv.v;
		rgb.g = t;
		rgb.b = p;
		break;
	case 1:
		rgb.r = q;
		rgb.g = hsv.v;
		rgb.b = p;
		break;
	case 2:
		rgb.r = p;
		rgb.g = hsv.v;
		rgb.b = t;
		break;
	case 3:
		rgb.r = p;
		rgb.g = q;
		rgb.b = hsv.v;
		break;
	case 4:
		rgb.r = t;
		rgb.g = p;
		rgb.b = hsv.v;
		break;
	default:		// case 5:
		rgb.r = hsv.v;
		rgb.g = p;
		rgb.b = q;
		break;
	}

	rgb.r *= 255.f;
	rgb.g *= 255.f;
	rgb.b *= 255.f;
	return rgb;
}

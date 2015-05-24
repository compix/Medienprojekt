#pragma once

struct RGB;
struct HSV;

namespace Convert
{
	HSV RGBtoHSV(RGB rgb);
	RGB HSVtoRGB(HSV hsv);
}

struct RGB
{
	RGB() {}

	/**
	* @brief	Values in [0, 255]
	*/
	RGB(float r, float g, float b) : r(r), g(g), b(b) {}

	float r;
	float g;
	float b;
};

struct HSV
{
	HSV() {}

	/**
	* @brief	Hue in [0, 360]°. s, v in [0, 100]%
	*/
	HSV(float h, float s, float v) : h(h), s(s), v(v) {}
	operator RGB() const { return Convert::HSVtoRGB(*this); }

	float h;       // angle in degrees
	float s;       // percent
	float v;       // percent
};


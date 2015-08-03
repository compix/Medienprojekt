#pragma once
#include "SFML/Graphics.hpp"

struct RGB;
struct HSV;

namespace Convert
{
	HSV RGBtoHSV(RGB rgb);
	RGB HSVtoRGB(HSV hsv);

	RGB sfColorToRGB(sf::Color color);
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

	static const RGB Red;
	static const RGB Green;
	static const RGB Blue;
	static const RGB Yellow;
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


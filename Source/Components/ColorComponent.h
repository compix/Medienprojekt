#pragma once
#include "../Utils/Colors.h"

/**
* @struct	ColorComponent
*
* @brief	Stores the color of the owner.
*/

struct ColorComponent
{
	ColorComponent(RGB color)
		: color(color) {}

	RGB color;
};
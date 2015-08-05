#pragma once
#include <ecstasy/core/Component.h>

/**
 * @struct	CellComponent
 *
 * @brief	Represents the cell in the grid the entity is currently in.
 */

struct CellComponent: public Component<CellComponent>
{
	CellComponent(uint8_t x = 0, uint8_t y = 0)
		: x(x), y(y) {}

	uint8_t x, y;
};
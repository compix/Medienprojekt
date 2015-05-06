#pragma once

/**
 * @struct	CellComponent
 *
 * @brief	Represents the cell in the grid the entity is currently in.
 */

struct CellComponent
{
	CellComponent(int x = 0, int y = 0)
		: x(x), y(y) {}

	int x, y;
};
#pragma once
#include <ecstasy/core/Component.h>

/**
 * @struct	CellComponent
 *
 * @brief	Represents the cell in the grid the entity is currently in.
 */

struct CellComponent: public ECS::Component<CellComponent>
{
	uint8_t x = 0, y = 0;
	
	void reset() override {
		x = y = 0;
	}
};
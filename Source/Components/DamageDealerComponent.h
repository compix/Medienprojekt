#pragma once

/**
 * @brief	Used for entities that can deal damage.
 */

struct DamageDealerComponent
{
	DamageDealerComponent(unsigned int damage) : damage(damage) {}

	unsigned int damage;
};
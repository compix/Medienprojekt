#pragma once
#include <ecstasy/core/Component.h>

/**
 * @brief	Used for entities that can deal damage.
 */

struct DamageDealerComponent: public Component<DamageDealerComponent>
{
	DamageDealerComponent(unsigned int damage) : damage(damage) {}

	unsigned int damage;
};
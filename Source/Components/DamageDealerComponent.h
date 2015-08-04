#pragma once
#include <ecstasy/core/Component.h>

/**
 * @brief	Used for entities that can deal damage.
 */

struct DamageDealerComponent: public Component<DamageDealerComponent>
{
	unsigned int damage = 0;
	
	void reset() override {
		damage = 0;
	}
};
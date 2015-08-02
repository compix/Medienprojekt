#pragma once
#include <ecstasy/core/Component.h>
#include "ecstasy/core/Entity.h"

/**
 * @brief	An entity can own another entity: A character owns a bomb for example.
 */

struct OwnerComponent: public ECS::Component<OwnerComponent>
{
	uint64_t entityId = 0;
	
	void reset() override {
		entityId = 0;
	}
};
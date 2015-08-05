#pragma once
#include <ecstasy/core/Component.h>
#include "ecstasy/core/Entity.h"

/**
 * @brief	An entity can own another entity: A character owns a bomb for example.
 */

struct OwnerComponent: public Component<OwnerComponent>
{
	OwnerComponent(uint64_t entityId) : entityId(entityId) {}

	uint64_t entityId;
};
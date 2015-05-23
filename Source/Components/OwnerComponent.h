#pragma once
#include <entityx/Entity.h>

/**
 * @brief	An entity can own another entity: A character owns a bomb for example.
 */

struct OwnerComponent
{
	OwnerComponent(entityx::Entity entity) : entity(entity) {}

	entityx::Entity entity;
};
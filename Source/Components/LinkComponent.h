#pragma once
#include <vector>
#include <entityx/Entity.h>

struct LinkComponent
{
	LinkComponent(bool dependent) : dependent(dependent) {}
	LinkComponent() : dependent(true) {}

	bool dependent; // If dependent then links will be destroyed as soon as the owner of this component is destroyed
	std::vector<entityx::Entity> links;
};
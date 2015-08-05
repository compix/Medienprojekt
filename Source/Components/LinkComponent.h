#pragma once
#include <ecstasy/core/Component.h>
#include <vector>

struct LinkComponent: public Component<LinkComponent>
{
	LinkComponent(bool dependent) : dependent(dependent) {}
	LinkComponent() : dependent(true) {}

	bool dependent; // If dependent then links will be destroyed as soon as the owner of this component is destroyed
	std::vector<uint64_t> links;
};
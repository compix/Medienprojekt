#pragma once
#include <ecstasy/core/Component.h>
#include <vector>
#include "ecstasy/core/Entity.h"

struct LinkComponent: public Component<LinkComponent>
{
	bool dependent = true; // If dependent then links will be destroyed as soon as the owner of this component is destroyed
	std::vector<uint64_t> links;
	
	void reset() override {
		dependent = true;
		links.clear();
	}
};
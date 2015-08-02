#pragma once
#include <ecstasy/core/Component.h>
#include "ecstasy/core/Entity.h"

struct PortalMarkerComponent: public ECS::Component<PortalMarkerComponent>
{
	uint64_t portalId = 0;
	
	void reset() override {
		portalId = 0;
	}
};
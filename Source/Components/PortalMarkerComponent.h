#pragma once
#include <ecstasy/core/Component.h>
#include "ecstasy/core/Entity.h"

struct PortalMarkerComponent: public Component<PortalMarkerComponent>
{
	PortalMarkerComponent(uint64_t portalId) : portalId(portalId) {}

	uint64_t portalId;
};
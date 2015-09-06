#pragma once
#include <entityx/entityx.h>

struct PortalComponent
{
	PortalComponent(){}

	entityx::Entity otherPortal;
};

#pragma once
#include "../entityx/entityx/entityx.h"

struct PortalComponent
{
	PortalComponent(){}

	entityx::Entity otherPortal;
};
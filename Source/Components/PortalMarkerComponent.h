#pragma once
#include <entityx/Entity.h>

struct PortalMarkerComponent
{
	PortalMarkerComponent(entityx::Entity::Id portalId) :portalId(portalId){}

	entityx::Entity::Id portalId;
};
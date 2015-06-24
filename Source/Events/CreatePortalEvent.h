#pragma once
#include <entityx/Entity.h>

struct CreatePortalEvent
{
	CreatePortalEvent(entityx::Entity triggerEntity)
		:triggerEntity(triggerEntity){}

	entityx::Entity triggerEntity;
};
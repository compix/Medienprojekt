#pragma once
#include <entityx/Entity.h>

struct HoldingEvent
{
	HoldingEvent(entityx::Entity whoHolds, entityx::Entity wantToHold)
		: whoHolds(whoHolds), bomb(wantToHold) {}

	entityx::Entity whoHolds;
	entityx::Entity bomb;
};

struct HoldingStatusEvent
{
	HoldingStatusEvent(entityx::Entity entity, bool holding)
		: entity(entity), holding(holding) {}

	entityx::Entity entity;
	bool holding;
};
#pragma once
#include <entityx/Entity.h>

struct HoldingEvent
{
	HoldingEvent(entityx::Entity whoHolds, entityx::Entity wantToHold)
		: whoHolds(whoHolds), wantToHold(wantToHold) {}

	entityx::Entity whoHolds;
	entityx::Entity wantToHold;
};

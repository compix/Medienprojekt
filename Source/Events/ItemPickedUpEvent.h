#pragma once
#include <entityx/Entity.h>

struct ItemPickedUpEvent
{
	ItemPickedUpEvent() {}
	ItemPickedUpEvent(entityx::Entity item, entityx::Entity itemReceiver) :item(item), itemReceiver(itemReceiver) {}

	entityx::Entity item;
	entityx::Entity itemReceiver; // the entity who picked up the item
};
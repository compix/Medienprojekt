#pragma once
#include "../Events/ItemCreatedEvent.h"

struct ItemSpawnerComponent
{
	ItemSpawnerComponent(ItemType itemType) : itemType(itemType) {}

	ItemType itemType;
};
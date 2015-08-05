#pragma once
#include <ecstasy/core/Component.h>

enum class ItemType : uint8_t
{
	BOMB_CAP_BOOST,
	BOMB_KICK_SKILL,
	SPEED_MULTIPLICATOR
};

struct ItemComponent: public Component<ItemComponent>
{
	ItemComponent() {}
	ItemComponent(const ItemType& type) : type(type) {}

	ItemType type;
};
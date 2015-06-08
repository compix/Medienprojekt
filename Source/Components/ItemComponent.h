#pragma once

enum class ItemType
{
	BOMB_CAP_BOOST
};

struct ItemComponent
{
	ItemComponent() {}
	ItemComponent(const ItemType& type) : type(type) {}

	ItemType type;
};
#pragma once

enum class ItemType
{
	BOMB_CAP_BOOST,
	BOMB_KICK_SKILL
};

struct ItemComponent
{
	ItemComponent() {}
	ItemComponent(const ItemType& type) : type(type) {}

	ItemType type;
};
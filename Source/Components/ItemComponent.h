#pragma once

enum class ItemType
{
	BOMB_CAP_BOOST,
	BOMB_KICK_SKILL,
	SPEED_MULTIPLICATOR
};

struct ItemComponent
{
	ItemComponent() {}
	ItemComponent(const ItemType& type) : type(type) {}

	ItemType type;
};
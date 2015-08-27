#pragma once

enum class ItemType : uint8_t
{
	BOMB_CAP_BOOST,
	BOMB_KICK_SKILL,
	SPEED_MULTIPLICATOR,
	BOMB_RANGE_BOOST,
	PORTAL_SKILL,
	ANTI_MAGNET_SKILL,
	PUNCH_SKILL,
	HOLD_BOMB_SKILL,
	BLINK_SKILL,
	GHOST_BOMB,
	LIGHTNING_BOMB,
	NONE
};

struct ItemComponent
{
	ItemComponent() {}
	ItemComponent(const ItemType& type) : type(type) {}

	ItemType type;
};
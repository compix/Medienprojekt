#pragma once
#include <ecstasy/core/Component.h>

enum class ItemType : uint8_t
{
	BOMB_CAP_BOOST,
	BOMB_KICK_SKILL,
	SPEED_MULTIPLICATOR
};

struct ItemComponent: public ECS::Component<ItemComponent>
{
	ItemType type = ItemType::BOMB_CAP_BOOST;
	
	void reset() override {
		type = ItemType::BOMB_CAP_BOOST;
	}
};
#include "Common.h"
#include <assert.h>
#include "../Components/ItemComponent.h"
#include "../Components/InventoryComponent.h"

Direction CommonUtil::toDirection(int dirX, int dirY)
{
	assert(dirX == 0 || dirY == 0);

	switch (dirX)
	{
	case 1:
		return Direction::RIGHT;
	case -1:
		return Direction::LEFT;
	default:
		switch (dirY)
		{
		case 1:
			return Direction::DOWN;
		case -1:
			return Direction::UP;
		}
	}

	// This should never happen -> obviously wrong input
	assert(false);
	return Direction::DOWN;
}

SkillType CommonUtil::toSkill(ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::PORTAL_SKILL: 
		return SkillType::PLACE_PORTAL;
	case ItemType::PUNCH_SKILL: 
		return SkillType::PUNCH;
	case ItemType::BLINK_SKILL: 
		return SkillType::BLINK;
	default:
		assert(false);
		return SkillType::NONE;
	}
}

BombType CommonUtil::toBomb(ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::GHOST_BOMB: 
		return BombType::GHOST;
	case ItemType::LIGHTNING_BOMB: 
		return BombType::LIGHTNING;
	default: 
		assert(false); // No such item type
		return BombType::NORMAL;
	}
}

ItemType CommonUtil::toItem(SkillType skillType)
{
	switch (skillType)
	{
	case SkillType::PLACE_PORTAL:
		return ItemType::PORTAL_SKILL;
	case SkillType::PUNCH:
		return ItemType::PUNCH_SKILL;
	case SkillType::BLINK: 
		return ItemType::BLINK_SKILL;
	default:
		assert(false);
		return ItemType::NONE;
	}
}

ItemType CommonUtil::toItem(BombType bombType)
{
	switch (bombType)
	{
	case BombType::GHOST:
		return ItemType::GHOST_BOMB;
	case BombType::LIGHTNING:
		return ItemType::LIGHTNING_BOMB;
	case BombType::LIGHTNING_PEAK:
		return ItemType::LIGHTNING_BOMB;
	default:
		assert(false);
		return ItemType::NONE;
	}
}

bool CommonUtil::isSkill(ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::PORTAL_SKILL:
	case ItemType::PUNCH_SKILL:
	case ItemType::BLINK_SKILL:
		return true;
	default:
		return false;;
	}
}

bool CommonUtil::isBomb(ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::GHOST_BOMB:
	case ItemType::LIGHTNING_BOMB:
		return true;
	default:
		return false;
	}
}
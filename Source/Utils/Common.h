#pragma once
#include<stdint.h>

enum class BombType : uint8_t;
enum class ItemType : uint8_t;
enum class SkillType : uint8_t;

enum class Direction : uint8_t
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct LevelCell
{
	uint8_t x;
	uint8_t y;
};

namespace CommonUtil
{
	Direction toDirection(int dirX, int dirY);

	SkillType toSkill(ItemType itemType);
	BombType toBomb(ItemType itemType);
	ItemType toItem(SkillType skillType);
	ItemType toItem(BombType bombType);

	bool isSkill(ItemType itemType);
	bool isBomb(ItemType itemType);
}

struct EnumClassHash
{
	template <typename T>
	size_t operator()(T t) const
	{
		return static_cast<size_t>(t);
	}
};

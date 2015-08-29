#pragma once
#include "../GameConstants.h"
#include <utility>
#include <entityx/Entity.h>
#include "../Utils/ActiveQueue.h"
#include "ItemComponent.h"
#include "../Utils/Common.h"

enum class SkillType : uint8_t
{
	NONE,
	PLACE_PORTAL,
	PUNCH,
	BLINK
};

/**
* Has to be used with the mapped key/button.
* Note: Skill priority is only different for timed skills like "Martial Arts". 
* Check out the wiki for more information: https://github.com/compix/Medienprojekt/wiki/Active-Skills
*/
struct Skill
{
	Skill(SkillType type) : type(type)
	{
		switch (type)
		{
		case SkillType::NONE:
			priority = 255;
			break;
		case SkillType::PLACE_PORTAL:
			priority = 10;
			break;
		case SkillType::PUNCH:
			priority = 10;
			break;
		case SkillType::BLINK:
			priority = 10;
			break;
		default:
			assert(false);
		}
	}

	inline bool operator==(const Skill& other) { return type == other.type; }

	SkillType type;
	uint8_t priority; // 0 = highest priority
};

struct SkillComparator
{
	inline bool operator()(const Skill& s1, const Skill& s2) { return s1.priority < s2.priority; }
};

enum class BombType : uint8_t
{
	NORMAL,
	GHOST, // Only stopped by solid blocks
	LIGHTNING, // 1 Range explosion at the end of the spread
	LIGHTNING_PEAK // Not a bomb, but used for explosions
};

struct BombTypeComparator
{
	// Always the same priority
	inline bool operator()(const BombType& b1, const BombType& b2) { return false; }
};

struct InventoryComponent
{
	InventoryComponent() : isHoldingBomb(false), placedBombCount(0)
	{
		if (GameConstants::INIT_PORTAL_SKILL)
			put(SkillType::PLACE_PORTAL);

		if (GameConstants::INIT_PUNCH_SKILL)
			put(SkillType::PUNCH);

		if (GameConstants::INIT_BLINK_SKILL)
			put(SkillType::BLINK);

		itemCounts[ItemType::BOMB_CAP_BOOST] = GameConstants::INIT_BOMB_COUNT;
		itemCounts[ItemType::BOMB_KICK_SKILL] = GameConstants::INIT_PLAYERS_CAN_KICK ? 1 : 0;
		itemCounts[ItemType::SPEED_MULTIPLICATOR] = 0;
		itemCounts[ItemType::BOMB_RANGE_BOOST] = GameConstants::INIT_BOMB_RANGE;
		itemCounts[ItemType::PORTAL_SKILL] = 0;
		itemCounts[ItemType::ANTI_MAGNET_SKILL] = GameConstants::INIT_ANTI_MAGNET ? 1 : 0;
		itemCounts[ItemType::PUNCH_SKILL] = 0;
		itemCounts[ItemType::HOLD_BOMB_SKILL] = GameConstants::INIT_PLAYERS_CAN_HOLD_BOMB ? 1 : 0;
		itemCounts[ItemType::BLINK_SKILL] = 0;
		itemCounts[ItemType::GHOST_BOMB] = 0;
		itemCounts[ItemType::LIGHTNING_BOMB] = 0;
	}

	bool isHoldingBomb;
	BombType holdingBombType;
	entityx::Entity holdingBombOwner;
	uint8_t placedBombCount;
	std::pair<entityx::Entity, entityx::Entity> placedPortals;
	ActiveQueue<Skill, SkillComparator> activeSkills; // Currently active skills with a custom priority order
	ActiveQueue<BombType, BombTypeComparator> activeBombs;
	// How many items the entity has of the given type
	std::unordered_map<ItemType, uint8_t, EnumClassHash> itemCounts;
	std::vector<ItemType> items;

	inline void put(SkillType skillType) { activeSkills.put(Skill(skillType)); }
	inline SkillType activeSkill() { return activeSkills.empty() ? SkillType::NONE : activeSkills.top().type; }
	inline bool isActive(SkillType type) { return activeSkill() == type; }
	inline void remove(SkillType type) { activeSkills.remove(Skill(type)); }

	inline BombType activeBomb() { return activeBombs.empty() ? BombType::NORMAL : activeBombs.top(); }
	inline void put(BombType bombType) { activeBombs.put(bombType); }
	inline void remove(BombType bombType) { activeBombs.remove(bombType); }

	inline float speed() { return (GameConstants::INIT_SPEED_MULTI + GameConstants::SPEED_MULTI_INC * itemCounts[ItemType::SPEED_MULTIPLICATOR]) * GameConstants::PLAYER_SPEED; }
	inline bool canPunchBomb() { return itemCounts[ItemType::PUNCH_SKILL] > 0; }
	inline bool canHoldBomb() { return itemCounts[ItemType::HOLD_BOMB_SKILL] > 0; }
	inline bool canKickBomb() { return itemCounts[ItemType::BOMB_KICK_SKILL] > 0; }
	inline bool hasAntiMagnet() { return itemCounts[ItemType::ANTI_MAGNET_SKILL] > 0; }
	inline uint8_t getExplosionRange() { return itemCounts[ItemType::BOMB_RANGE_BOOST]; }
	inline uint8_t getAvailableBombCount()
	{
		int c = itemCounts[ItemType::BOMB_CAP_BOOST] - placedBombCount;
		return uint8_t(c < 0 ? 0 : c);
	}
};
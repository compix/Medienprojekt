#pragma once
#include "../GameConstants.h"
#include <utility>
#include <entityx/Entity.h>
#include "../Utils/ActiveQueue.h"

enum class SkillType
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
	LIGHTNING // 1 Range explosion at the end of the spread
};

struct BombTypeComparator
{
	// Always the same priority
	inline bool operator()(const BombType& b1, const BombType& b2) { return false; }
};

struct InventoryComponent
{
	InventoryComponent() : bombCount(GameConstants::INIT_BOMB_COUNT),
		explosionRange(GameConstants::INIT_BOMB_RANGE),
		speedMultiplicator(GameConstants::INIT_SPEED_MULTI),
		bombKick(GameConstants::INIT_PLAYERS_CAN_KICK),
		antiMagnet(GameConstants::INIT_ANTI_MAGNET),
		isHoldingBomb(false),
		canHold(GameConstants::INIT_PLAYERS_CAN_HOLD_BOMB)
	{
		if (GameConstants::INIT_PORTAL_SKILL)
			put(SkillType::PLACE_PORTAL);

		if (GameConstants::INIT_PUNCH_SKILL)
			put(SkillType::PUNCH);

		if (GameConstants::INIT_BLINK_SKILL)
			put(SkillType::BLINK);
	}

	int bombCount;
	int explosionRange;
	float speedMultiplicator;
	bool bombKick;
	bool antiMagnet;
	bool isHoldingBomb;
	bool canHold;
	std::pair<entityx::Entity, entityx::Entity> placedPortals;
	ActiveQueue<Skill, SkillComparator> activeSkills; // Currently active skills with a custom priority order
	ActiveQueue<BombType, BombTypeComparator> activeBombs;

	inline void put(SkillType skillType) { activeSkills.put(Skill(skillType)); }
	inline SkillType activeSkill() { return activeSkills.empty() ? SkillType::NONE : activeSkills.top().type; }
	inline bool isActive(SkillType type) { return activeSkill() == type; }
	inline void removeSkill(SkillType type) { activeSkills.remove(Skill(type)); }

	inline BombType activeBomb() { return activeBombs.empty() ? BombType::NORMAL : activeBombs.top(); }
	inline void put(BombType bombType) { activeBombs.put(bombType); }
	inline bool isGhostBombActive() { return activeBomb() == BombType::GHOST; }
	inline bool isLightningBombActive() { return activeBomb() == BombType::LIGHTNING; }
};
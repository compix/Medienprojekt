#pragma once
#include "../GameConstants.h"
#include <utility>
#include <entityx/Entity.h>
#include <queue>
#include "../Utils/Common.h"

enum class SkillType
{
	NONE,
	PLACE_PORTAL
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
		default:
			assert(false);
		}
	}

	SkillType type;
	uint8_t priority; // 0 = highest priority
};

struct SkillComparator
{
	bool operator()(const Skill& s1, const Skill& s2)
	{
		return s1.priority > s2.priority;
	}
};

struct InventoryComponent
{
	InventoryComponent() : bombCount(GameConstants::INIT_BOMB_COUNT), 
		explosionRange(GameConstants::INIT_BOMB_RANGE), 
		speedMultiplicator(GameConstants::INIT_SPEED_MULTI), 
		bombKick(GameConstants::INIT_PLAYERS_CAN_KICK), 
		antiMagnet(GameConstants::INIT_ANTI_MAGNET)
	{
		activeSkills.push(Skill(SkillType::NONE));

		if (GameConstants::INIT_PORTAL_SKILL)
			activeSkills.push(Skill(SkillType::PLACE_PORTAL));
	}

	int bombCount;
	int explosionRange;
	float speedMultiplicator;
	bool bombKick;
	bool antiMagnet;
	std::pair<entityx::Entity, entityx::Entity> placedPortals;
	std::unordered_map<SkillType, bool, EnumClassHash> ownedSkills;		// All the skills the entity has, only skills that are used with a key/button. Placing bombs excluded.
	std::priority_queue<Skill, std::vector<Skill>, SkillComparator> activeSkills; // Currently active skills with a custom priority order

	inline bool has(SkillType skillType) { return ownedSkills.count(skillType) > 0; }

	void put(SkillType skillType)
	{
		if(!has(skillType))
		{
			activeSkills.push(Skill(skillType));
			ownedSkills[skillType] = true;
		}
	}

	inline SkillType activeSkill() { assert(activeSkills.size() > 0); return activeSkills.top().type; }
	inline bool isActive(SkillType type) { return activeSkill() == type; }
};
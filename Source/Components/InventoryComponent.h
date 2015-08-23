#pragma once
#include "../GameConstants.h"
#include <utility>
#include <entityx/Entity.h>
#include <algorithm>

enum class SkillType
{
	NONE,
	PLACE_PORTAL,
	PUNCH
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
		return s1.priority < s2.priority;
	}
};

class SkillQueue
{
public:
	SkillQueue()
	{
		m_skills.push_back(Skill(SkillType::NONE));
	}

	void put(SkillType skillType)
	{
		remove(skillType);
		auto it = std::lower_bound(m_skills.begin(), m_skills.end(), Skill(skillType), SkillComparator());
		m_skills.insert(it, Skill(skillType));
	}

	void remove(SkillType skillType)
	{
		assert(skillType != SkillType::NONE);
		for (auto it = m_skills.begin(); it != m_skills.end(); ++it)
		{
			if (it->type == skillType)
			{
				m_skills.erase(it);
				break;
			}
		}
	}

	// Skill set is never empty. If the entity has no skills then SkillType::NONE is returned.
	inline const Skill& top() { return *m_skills.begin(); }

private:
	std::vector<Skill> m_skills;
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
			activeSkills.put(SkillType::PLACE_PORTAL);

		if (GameConstants::INIT_PUNCH_SKILL)
			activeSkills.put(SkillType::PUNCH);
	}

	int bombCount;
	int explosionRange;
	float speedMultiplicator;
	bool bombKick;
	bool antiMagnet;
	bool isHoldingBomb;
	bool canHold;
	std::pair<entityx::Entity, entityx::Entity> placedPortals;
	SkillQueue activeSkills; // Currently active skills with a custom priority order
	

	inline void put(SkillType skillType) { activeSkills.put(skillType); }
	inline SkillType activeSkill() { return activeSkills.top().type; }
	inline bool isActive(SkillType type) { return activeSkill() == type; }
	inline void removeSkill(SkillType type) { activeSkills.remove(type); }
};
#pragma once
#include "../../Components/InventoryComponent.h"

/**
* Personality makes AI vary to make it more human.
*/
struct AIPersonality
{
	AIPersonality()
	{
		affinity.attackEnemy = 5.f;
		affinity.destroyBlock = 0.f;
		affinity.getSafe = 1.f;
		affinity.placePortal = 1.f;
		affinity.wait = 1.f;
		affinity.getItem = 5.f;
	}

	/**
	* Desires partly determine AI actions to create "personality".
	* Multiplicative path rating factors which can change over time by per seconds values or events.
	* Desire levels are in [0, FLT_MAX]
	*
	* Affinity is the base desire level for certain actions of an AI. 
	* Added as a constant to the path ratings.
	* Affinity levels are in [0, FLT_MAX]
	*/
	struct AIDesires
	{
		AIDesires() : destroyBlock(1.f), wait(1.f), attackEnemy(1.f), placePortal(1.f), getItem(1.f), getSafe(1.f) {}

		float destroyBlock;
		float wait;
		float attackEnemy;
		float placePortal;
		float getItem;
		float getSafe;
	} affinity, desires, changePerSecond;

	std::vector<SkillType> favoriteSkills;
	std::string name;

	inline void addFavorite(SkillType skillType) { if(!hasFavorite(skillType)) favoriteSkills.push_back(skillType); }

	bool hasFavorite(SkillType skillType);
};
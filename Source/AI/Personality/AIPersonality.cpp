#include "AIPersonality.h"

bool AIPersonality::hasFavorite(SkillType skillType)
{
	for (auto st : favoriteSkills)
		if (st == skillType)
			return true;
	return false;
}
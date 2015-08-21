#include "RateCombination.h"

RateCombination::RateCombination(std::initializer_list<PathRating> ratings)
{
	m_pathRatings = ratings;
}

bool RateCombination::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)
{
	float totalRating = 0.f;
	for (auto& rating : m_pathRatings)
	{
		bool valid = rating(pathEngine, path, entity, taskNum);

		if (valid)
			totalRating += path.rating;
		else
			return false;
	}

	path.rating = totalRating;
	return true;
}
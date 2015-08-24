#include "RateCombination.h"

RateCombination::RateCombination(std::initializer_list<PathRating> ratings)
{
	m_pathRatings = ratings;
	assert(m_pathRatings.size() > 0);
}

bool RateCombination::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	float totalRating = 0.f;
	for (auto& rating : m_pathRatings)
	{
		bool valid = rating(pathEngine, path, entity);

		if (valid)
			totalRating += path.rating;
		else
			return false;
	}

	path.rating = totalRating / m_pathRatings.size();
	return true;
}
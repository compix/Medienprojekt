#include "RateCombination.h"

RateCombination::RateCombination(std::initializer_list<PathRating> ratings)
{
	m_pathRatings = ratings;
}

bool RateCombination::operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum)
{
	pathEngine->makePath(pathOut, node, taskNum);
	pathOut.rating = 0.f;
	for (auto& rating : m_pathRatings)
	{
		AIPath path;
		bool valid = rating(pathEngine, node, path, taskNum);

		if (valid)
			pathOut.rating += path.rating;
		else
			return false;
	}

	return true;
}
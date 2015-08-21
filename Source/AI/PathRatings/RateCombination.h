#pragma once
#include <vector>
#include "../Behaviors/FollowPath.h"

/**
* Combines multiple ratings by adding them. If a path doesn't pass on one rating the other ratings aren't considered anymore
* and the whole Combination "fails" (false is returned).
*
* Combinations can make an AI aware of multiple factors. Example: Does the path lead to an item? Is it safe? How high is the risk to be trapped?
* The combination will consider all 3 questions and sum their ratings if they are satisfied. Then the best rated path will be chosen in the
* PathEngine algorithm.
*/
class RateCombination
{
public:
	RateCombination(std::initializer_list<PathRating> ratings);

	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

private:
	std::vector<PathRating> m_pathRatings;
};
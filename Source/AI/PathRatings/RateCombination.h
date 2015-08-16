#pragma once
#include <cinttypes>
#include <tuple>
#include <utility>
#include <type_traits>
#include "../PathFinding/PathEngine.h"

using std::enable_if;

class PathEngine;
struct GraphNode;

/* BUG IN VS2013 - Workaround below - This will work in VS2015
template<std::size_t num = 0, class... Ratings>
inline typename std::enable_if<num == sizeof...(Ratings), bool>::type evaluateAll(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum, std::tuple<Ratings...>& ratings)
{
	return true;
}

template<std::size_t num = 0, class... Ratings>
inline typename std::enable_if < num < sizeof...(Ratings), bool>::type evaluateAll(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum, std::tuple<Ratings...>& ratings)
{
	Path path;
	bool valid = std::get<num>(ratings)(pathEngine, node, path, taskNum);

	if (valid)
		pathOut.rating += path.rating;

	return valid && evaluateAll<num + 1, Ratings...>(pathEngine, node, pathOut, taskNum, ratings);
}*/

namespace Rate
{
	template<std::size_t num = 0, class... Ratings, typename = typename std::enable_if<num == sizeof...(Ratings), bool>::type, int = 0>
	inline bool evaluateAll(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum, std::tuple<Ratings...>& ratings)
	{
		return true;
	}

	// num < sizeof...(Ratings) won't work because VS2013 Compiler is bugged
	template<std::size_t num = 0, class... Ratings, typename = typename std::enable_if<num != sizeof...(Ratings), bool>::type>
	inline bool evaluateAll(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum, std::tuple<Ratings...>& ratings)
	{
		AIPath path;
		bool valid = std::get<num>(ratings)(pathEngine, node, path, taskNum);

		if (valid)
			pathOut.rating += path.rating;

		return valid && evaluateAll<num + 1, Ratings...>(pathEngine, node, pathOut, taskNum, ratings);
	}
}

/**
* Combines multiple ratings by adding them. If a path doesn't pass on one rating the other ratings aren't considered anymore
* and the whole Combination "fails" (false is returned).
*
* Combinations can make an AI aware of multiple factors. Example: Does the path lead to an item? Is it safe? How high is the risk to be trapped?
* The combination will consider all 3 questions and sum their ratings if they are satisfied. Then the best rated path will be chosen in the
* PathEngine algorithm.
*/
template<class T1, class T2, class... Args>
class RateCombination
{
public:
	RateCombination(T1 rating1, T2 rating2, Args... moreRatings);

	bool operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum);

private:
	std::tuple<T1, T2, Args...> m_ratings;
};

template <class T1, class T2, class ... Args>
RateCombination<T1, T2, Args...>::RateCombination(T1 rating1, T2 rating2, Args... moreRatings)
	:m_ratings(rating1, rating2, moreRatings...)
{
}

template <class T1, class T2, class ... Args>
bool RateCombination<T1, T2, Args...>::operator()(PathEngine* pathEngine, GraphNode* node, AIPath& pathOut, uint8_t taskNum)
{
	pathEngine->makePath(pathOut, node, taskNum);
	pathOut.rating = 0.f;
	return Rate::evaluateAll(pathEngine, node, pathOut, taskNum, m_ratings);
}
#pragma once
#include "../PathFinding/PathEngine.h"

/**
* This rating doesn't guarantee safety but can be used in unsafe situations considering best case scenarios.
* RateSafety is used for guaranteed safety. (worst case)
*/
class RateRiskySafety
{
public:
	RateRiskySafety();

	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
private:
	float m_exploTimeInfluence;
	float m_pathDurationInfluence;
};
#pragma once
#include "../PathFinding/PathEngine.h"

class RateSafety
{
public:
	RateSafety();

	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);
private:
	float m_exploTimeInfluence;
	float m_pathDurationInfluence;
};
#pragma once
#include "PathRatings/PathRating.h"
#include "Behaviors/Behavior.h"
#include "PathFinding/AIPath.h"

class Action
{
public:

private:
	PathRating m_pathRating;
	AIPath m_path;
	Behavior m_behavior;
};
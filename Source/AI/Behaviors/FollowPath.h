#pragma once
#include "Behavior.h"
#include "../PathFinding/PathEngine.h"
#include <stdint.h>

class FollowPath
{
public:
	FollowPath(const AIPath& path, LayerManager* layerManager);

	inline void setPath(const AIPath& path) { m_path = path; }
	void operator ()(entityx::Entity& entity);

protected:
	AIPath m_path;
	LayerManager* m_layerManager;
};
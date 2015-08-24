#pragma once
#include "Behavior.h"
#include "../PathFinding/PathEngine.h"

class FollowPath
{
public:
	FollowPath();
	explicit FollowPath(LayerManager* layerManager);
	FollowPath(const AIPath& path, LayerManager* layerManager);

	inline void setPath(const AIPath& path) { m_path = path; }
	inline AIPath& path() { return m_path; }
	bool operator()(entityx::Entity& entity);

protected:
	AIPath m_path;
	LayerManager* m_layerManager;
};
#pragma once
#include "Behavior.h"
#include "../PathFinding/PathEngine.h"
#include <stdint.h>

class FollowPath : public Behavior
{
public:
	explicit FollowPath(const AIPath& path, LayerManager* layerManager);

	inline void setPath(const AIPath& path) { m_path = path; }
	void update(entityx::Entity& entity) override;

protected:
	AIPath m_path;
	LayerManager* m_layerManager;
};
#pragma once
#include "Behavior.h"
#include "../Utils/PathFinding/PathEngine.h"
#include <stdint.h>

class FollowPath : public Behavior
{
public:
	explicit FollowPath(Path path, LayerManager* layerManager);

	inline void setPath(Path path) { m_path = path; }
	void update(entityx::Entity& entity) override;

protected:
	Path m_path;
	LayerManager* m_layerManager;
};
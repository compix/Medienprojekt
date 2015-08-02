#pragma once

#pragma once
#include <ecstasy/core/EntitySystem.h>
#include "../Utils/PathFinding/PathEngine.h"

class PathEngine;

class AISystem : public EntitySystem<AISystem>
{
public:
	AISystem(PathEngine* pathEngine);
	void update(float dt) override;

private:
	PathEngine* m_pathEngine;
	Path m_path;
};


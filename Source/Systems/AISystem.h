#pragma once

#pragma once
#include <entityx/entityx.h>
#include "../Utils/PathFinding/PathEngine.h"

class PathEngine;

class AISystem : public entityx::System<AISystem>
{
public:
	AISystem(PathEngine* pathEngine);
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

private:
	PathEngine* m_pathEngine;
	Path m_path;
};


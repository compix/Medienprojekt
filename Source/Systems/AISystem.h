#pragma once
#include <ecstasy/core/EntitySystem.h>
#include <ecstasy/core/Engine.h>
#include "../Utils/PathFinding/PathEngine.h"

class PathEngine;

class AISystem : public EntitySystem<AISystem>
{
public:
	AISystem(PathEngine* pathEngine);
	void update(float dt) override;
	void addedToEngine(Engine *engine) override;

private:
	PathEngine* m_pathEngine;
	Path m_path;
	const std::vector<Entity *> *m_nonAIPlayers;
	const std::vector<Entity *> *m_AIPlayers;
};


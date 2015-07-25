#pragma once

#pragma once
#include <entityx/entityx.h>
#include "../Utils/PathFinding/PathEngine.h"



class AISystem : public entityx::System<AISystem>
{
public:
	AISystem(PathEngine* pathEngine);
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void visualize();

	void getEnemies(Entity self, std::vector<Entity>& outEnemies);

	void getCloseEnemies(Entity self, std::vector<Entity>& outEnemies);

private:
	PathEngine* m_pathEngine;
	Path m_path;
	Path m_safePath;

	Path m_lastPath;
};
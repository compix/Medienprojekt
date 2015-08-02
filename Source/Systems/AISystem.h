#pragma once

#pragma once
#include <entityx/entityx.h>
#include "../Utils/PathFinding/PathEngine.h"



class AISystem : public entityx::System<AISystem>
{
public:
	AISystem(LayerManager* layerManager);
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void visualize();

	void getEnemies(Entity self, std::vector<Entity>& outEnemies);

	void getCloseEnemies(Entity self, std::vector<Entity>& outEnemies);

	void init();
private:
	std::unique_ptr<PathEngine> m_pathEngine;
	LayerManager* m_layerManager;
};
#pragma once
#include <entityx/entityx.h>
#include "../AI/PathFinding/PathEngine.h"
#include "../Utils/Logging/Logger.h"
#include "../AI/Actions/ActionType.h"
#include "../AI/AIVisualizer.h"

class AISystem : public entityx::System<AISystem>, public entityx::Receiver<AISystem>
{
public:
	AISystem(LayerManager* layerManager);
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void configure(entityx::EventManager& eventManager) override;
	void receive(const entityx::EntityDestroyedEvent& destroyedEvent);

	static void getEnemies(Entity self, std::vector<Entity>& outEnemies);

	void getCloseEnemies(Entity self, std::vector<Entity>& outEnemies);

	void init();
	void reset();
private:
	void logAction(LogServiceId serviceId, ActionType action, AIPath& path);

private:
	std::unique_ptr<PathEngine> m_pathEngine;
	LayerManager* m_layerManager;

	float m_updateTimer;
	bool m_invalidPaths;

	AIVisualizer m_visualizer;
};
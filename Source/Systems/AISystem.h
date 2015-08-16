#pragma once
#include <entityx/entityx.h>
#include "../AI/PathFinding/PathEngine.h"
#include "../Utils/Logging/Logger.h"

enum class AIActionType
{
	PORTAL_PATH,
	WAIT_PATH,
	ITEM_PATH,
	DESTROY_BLOCK_PATH,
	SAFE_PATH,
	KICK_BOMB_PATH,
	DESPERATE_SAFE_PATH,
	PLACING_BOMB,
	PLACING_PORTAL
};

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
	void logAction(LogServiceId serviceId, AIActionType action);
private:
	std::unique_ptr<PathEngine> m_pathEngine;
	LayerManager* m_layerManager;

	float m_updateTimer;
};
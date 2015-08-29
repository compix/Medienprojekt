#pragma once
#include <entityx/entityx.h>
#include "../AI/PathFinding/PathEngine.h"
#include "../AI/AIVisualizer.h"
#include "../GameGlobals.h"

struct BombCreatedEvent;
struct PortalCreatedEvent;
struct DeathEvent;

#define AI_LOGGING

class AISystem : public entityx::System<AISystem>, public entityx::Receiver<AISystem>
{
public:
	AISystem(LayerManager* layerManager);
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void configure(entityx::EventManager& eventManager) override;
	void receive(const DeathEvent& deathEvent);
	void receive(const BombCreatedEvent& bombCreatedEvent);
	void receive(const PortalCreatedEvent& portalCreatedEvent);

	void init();
	void reset();
private:
	void log(entityx::Entity& entity);
	void log(entityx::Entity& entity, const std::string& txt);

	void updateAIDesires(entityx::Entity& entity, float deltaTime);
private:
	std::unique_ptr<PathEngine> m_pathEngine;
	LayerManager* m_layerManager;

	float m_updateTimer;

	AIVisualizer m_visualizer;
	bool m_forceRecomputation;
};
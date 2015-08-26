#pragma once
#include <entityx/entityx.h>
#include "../AI/PathFinding/PathEngine.h"
#include "../AI/AIVisualizer.h"

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

	static void getEnemies(Entity self, std::vector<Entity>& outEnemies);

	void getCloseEnemies(Entity self, std::vector<Entity>& outEnemies);

	void init();
	void reset();

	template<class C, class... Args>
	bool doEntitiesExistWithComponents();
private:
	void log(entityx::Entity& entity);
	void log(entityx::Entity& entity, const std::string& txt);
private:
	std::unique_ptr<PathEngine> m_pathEngine;
	LayerManager* m_layerManager;

	float m_updateTimer;

	AIVisualizer m_visualizer;
};

template <class C, class ... Args>
bool AISystem::doEntitiesExistWithComponents()
{
	for (auto entity : GameGlobals::entities->entities_with_components<C, Args...>())
		return true;

	return false;
}
#pragma once
#include <entityx/entityx.h>
#include "../AI/PathFinding/PathEngine.h"
#include "../Utils/Logging/Logger.h"
#include "../AI/Actions/ActionType.h"
#include "../AI/Actions/Action.h"

class AISystem : public entityx::System<AISystem>, public entityx::Receiver<AISystem>
{
public:
	AISystem(LayerManager* layerManager);
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void configure(entityx::EventManager& eventManager) override;
	void receive(const entityx::EntityDestroyedEvent& destroyedEvent);

	void visualize(entityx::EntityManager& entityManager);

	static void getEnemies(Entity self, std::vector<Entity>& outEnemies);

	void getCloseEnemies(Entity self, std::vector<Entity>& outEnemies);

	void init();
	void reset();
private:
	void logAction(LogServiceId serviceId, ActionType action, AIPath& path);

	void visualizeAction(ActionType actionType, entityx::Entity& entity);
private:
	std::unique_ptr<PathEngine> m_pathEngine;
	LayerManager* m_layerManager;

	float m_updateTimer;
	bool m_invalidPaths;

	// For visualization
	bool m_visualizeNodes;
	bool m_visualizeProperties;
	bool m_visualizePathInfo;
	bool m_visualizeDangerzones;
	bool m_visualizeActions;
	bool m_keyPressed;

	sf::Font m_font;
	sf::Text m_text;
};
#include "AISystem.h"
#include "../Components/AIComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/InventoryComponent.h"
#include "../AI/PathFinding/PathEngine.h"
#include "../GameConstants.h"
#include "../Utils/Random.h"
#include "../Events/ItemPickedUpEvent.h"
#include "../Game.h"
#include "../Components/DestructionComponent.h"
#include "../Components/BlockComponent.h"
#include "../AI/PathRatings/RateSafety.h"
#include "../AI/PathRatings/RateItem.h"
#include "../AI/PathRatings/RateDestroyBlockSpot.h"
#include "../AI/PathRatings/RateEscape.h"
#include "../AI/PathRatings/RateCombination.h"
#include "../AI/PathRatings/RateTrapDanger.h"
#include "../AI/PathRatings/RateDesperateSaveAttempt.h"
#include "../AI/PathRatings/RateDistanceToItems.h"
#include "../AI/PathRatings/RateDistanceToAffectedBlocks.h"
#include "../AI/PathRatings/RateKickBomb.h"
#include "../AI/PathRatings/RatePortalSpot.h"
#include "../Utils/Logging/Logger.h"
#include <sstream>
#include "../AI/AIUtil.h"
#include "../AI/Behaviors/PlaceBomb.h"
#include "../AI/Behaviors/UseSkill.h"
#include "../AI/Behaviors/FollowPath.h"
#include "../AI/Behaviors/DoNothing.h"
#include "../Components/ColorComponent.h"
#include "../AI/Actions/ActionSelector.h"
#include "../AI/Actions/GetSafe.h"
#include "../Components/PortalComponent.h"
#include "../Events/ExitEvent.h"

AISystem::AISystem(LayerManager* layerManager)
	: m_layerManager(layerManager), m_updateTimer(GameConstants::AI_UPDATE_TIME), m_invalidPaths(false),
	m_visualizeNodes(false), m_visualizeProperties(false), m_visualizePathInfo(false), m_visualizeDangerzones(false), m_keyPressed(false), m_visualizeActions(false)
{
	m_pathEngine = std::make_unique<PathEngine>(layerManager);

	if (!m_font.loadFromFile("Assets/fonts/DejaVuSans.ttf"))
	{
		std::cout << "Failed to load font Assets/fonts/DejaVuSans.ttf" << std::endl;
		GameGlobals::events->emit<ExitEvent>();
		return;
	}

	m_text.setFont(m_font);
	m_text.setCharacterSize(16);
	m_text.setColor(sf::Color::White);
}

void AISystem::init()
{
	for (auto entity : GameGlobals::entities->entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto aiComponent = entity.component<AIComponent>();

		PathRating destroyBlockRating = RateCombination({ RateDestroyBlockSpot(), RateEscape(), RateTrapDanger(true), RateDistanceToItems() });
		aiComponent->actions[ActionType::DESTROY_BLOCK] = std::make_shared<Action>(m_pathEngine.get(), destroyBlockRating, PlaceBomb(), m_layerManager);

		PathRating waitRating = RateCombination({ RateSafety(), RateDistanceToAffectedBlocks() });
		aiComponent->actions[ActionType::WAIT] = std::make_shared<Action>(m_pathEngine.get(), waitRating, DoNothing(), m_layerManager);

		PathRating getItemRating = RateCombination({ RateSafety(), RateItem(), RateTrapDanger() });
		aiComponent->actions[ActionType::GET_ITEM] = std::make_shared<Action>(m_pathEngine.get(), getItemRating, DoNothing(), m_layerManager);

		aiComponent->actions[ActionType::GET_SAFE] = std::make_shared<GetSafe>(m_pathEngine.get(), m_layerManager);

		PathRating placePortalRating = RateCombination({ RateSafety(), RatePortalSpot() });
		aiComponent->actions[ActionType::PLACE_PORTAL] = std::make_shared<Action>(m_pathEngine.get(), placePortalRating, UseSkill(), m_layerManager);
	}
}

void AISystem::reset()
{
	m_pathEngine->reset();
}

void AISystem::logAction(LogServiceId serviceId, ActionType action, AIPath& path)
{
	std::stringstream ss;

	switch (action)
	{
	case ActionType::DESTROY_BLOCK:
		ss << "Destroying block.";	
		break;
	case ActionType::WAIT:
		ss << "Waiting for opportunities.";
		break;
	case ActionType::GET_ITEM:
		ss << "Getting item.";
		break;
	case ActionType::GET_SAFE:
		ss << "Getting safe.";
		break;
	case ActionType::KICK_BOMB:
		ss << "Kicking bomb.";
		break;
	case ActionType::TRY_TO_SURVIVE:
		ss << "Trying to survive.";
		break;
	case ActionType::PLACE_PORTAL:
		ss << "Placing portal.";
		break;
	default:
		ss << "UNKNOWN ACTION.";
		break;
	}

	if (path.goal())
	{
		ss << " Goal: x = " << static_cast<int>(path.goal()->x) << " y = " << static_cast<int>(path.goal()->y);
	}

	Logger::log(ss.str(), serviceId);
}

void AISystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	m_updateTimer -= dt;

	m_pathEngine->update(static_cast<float>(dt));

	for (auto entity : entityManager.entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto cell = entity.component<CellComponent>();
		auto aiComponent = entity.component<AIComponent>();
		auto input = entity.component<InputComponent>();
		auto color = entity.component<ColorComponent>();

		// Reset inputs
		input->moveX = 0.f;
		input->moveY = 0.f;
		input->bombButtonPressed = false;
		input->skillButtonPressed = false;

		if (m_invalidPaths || !aiComponent->currentAction || aiComponent->currentAction->done() || !aiComponent->currentAction->valid(entity))
		{
			ActionPtr bestAction;
			ActionType bestActionType;

			// Prepare path and choose the best action
			for (auto& pair : aiComponent->actions)
			{
				auto& action = pair.second;
				action->preparePath(entity);

				if (!bestAction || bestAction->getRating() < action->getRating())
				{
					bestAction = action;
					bestActionType = pair.first;
				}
			}

			assert(bestAction);
			aiComponent->currentAction = bestAction.get();

			// Logging
			std::stringstream ss;
			ss << "AI/Entity" << color->color.str() << ".log";
			LogServiceId logId = Logger::requestService(ss.str());

			logAction(logId, bestActionType, aiComponent->currentAction->path());

			m_invalidPaths = false;
			aiComponent->currentActionType = bestActionType;
		}

		if (aiComponent->currentAction->path().nodes.size() == 0)
		{
			// No viable action
			continue;
		}

		aiComponent->currentAction->update(entity, static_cast<float>(dt));	
	}

	visualize(entityManager);

	if (m_updateTimer <= 0.f)
		m_updateTimer = GameConstants::AI_UPDATE_TIME;
}

void AISystem::configure(entityx::EventManager& eventManager)
{
	eventManager.subscribe<entityx::EntityDestroyedEvent>(*this);
}

void AISystem::receive(const entityx::EntityDestroyedEvent& destroyedEvent)
{
	auto entity = destroyedEvent.entity;
	if (entity.has_component<PortalComponent>())
	{
		// If portals disappear then paths can become invalid -> easy fix: force a recomputation
		m_invalidPaths = true;
	}
}

void AISystem::visualizeAction(ActionType actionType, entityx::Entity& entity)
{
	auto transform = entity.component<TransformComponent>();
	m_text.setPosition(transform->x - GameConstants::CELL_WIDTH * 0.5f, transform->y - GameConstants::CELL_HEIGHT);

	switch (actionType)
	{
	case ActionType::DESTROY_BLOCK:
		m_text.setString("Destroying block.");
		break;
	case ActionType::WAIT:
		m_text.setString("Waiting for opportunities.");
		break;
	case ActionType::GET_ITEM:
		m_text.setString("Getting item.");
		break;
	case ActionType::GET_SAFE:
		m_text.setString("Getting safe.");
		break;
	case ActionType::KICK_BOMB:
		m_text.setString("Kicking bomb.");
		break;
	case ActionType::TRY_TO_SURVIVE:
		m_text.setString("Trying to survive.");
		break;
	case ActionType::PLACE_PORTAL:
		m_text.setString("Placing portal.");
		break;
	default:
		m_text.setString("No idea what I'm doing.");
		break;
	}

	GameGlobals::window->draw(m_text);
}

void AISystem::visualize(entityx::EntityManager& entityManager)
{
	if (!m_keyPressed)
	{
		m_visualizeNodes = sf::Keyboard::isKeyPressed(sf::Keyboard::F1) ? !m_visualizeNodes : m_visualizeNodes;
		m_visualizePathInfo = sf::Keyboard::isKeyPressed(sf::Keyboard::F2) ? !m_visualizePathInfo : m_visualizePathInfo;
		m_visualizeDangerzones = sf::Keyboard::isKeyPressed(sf::Keyboard::F3) ? !m_visualizeDangerzones : m_visualizeDangerzones;
		m_visualizeProperties = sf::Keyboard::isKeyPressed(sf::Keyboard::F4) ? !m_visualizeProperties : m_visualizeProperties;
		m_visualizeActions = sf::Keyboard::isKeyPressed(sf::Keyboard::F5) ? !m_visualizeActions : m_visualizeActions;

		m_keyPressed = true;
	}

	m_keyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::F1) || sf::Keyboard::isKeyPressed(sf::Keyboard::F2) || 
		sf::Keyboard::isKeyPressed(sf::Keyboard::F3) || sf::Keyboard::isKeyPressed(sf::Keyboard::F4) || sf::Keyboard::isKeyPressed(sf::Keyboard::F5);

	m_pathEngine->visualize(m_visualizeNodes, m_visualizePathInfo, m_visualizeDangerzones, m_visualizeProperties);

	if (m_visualizeActions)
	{
		for (auto entity : entityManager.entities_with_components<AIComponent>())
		{
			auto aiComponent = entity.component<AIComponent>();
			m_pathEngine->visualize(aiComponent->currentAction->path());
			visualizeAction(aiComponent->currentActionType, entity);
		}
	}
}

void AISystem::getEnemies(Entity self, std::vector<Entity>& outEnemies)
{
	outEnemies.clear();
	for(auto e : GameGlobals::entities->entities_with_components<InventoryComponent>())
	{
		if (e != self)
			outEnemies.push_back(e);
	}
}

void AISystem::getCloseEnemies(Entity self, std::vector<Entity>& outEnemies)
{
	assert(self.valid() && self.has_component<CellComponent>() && self.has_component<InventoryComponent>());

	auto selfCell = self.component<CellComponent>();
	outEnemies.clear();

	for (auto e : GameGlobals::entities->entities_with_components<InventoryComponent, CellComponent>())
	{
		auto cell = e.component<CellComponent>();
		uint32_t distance = abs(selfCell->x - cell->x) + abs(selfCell->y - cell->y);
		if (e != self && distance < 4)
			outEnemies.push_back(e);
			
	}
}